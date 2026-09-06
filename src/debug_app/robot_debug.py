#!/usr/bin/env python3
"""Monitor de telemetria en tiempo real para el robot WRO.

Solo usa la biblioteca estandar de Python. Recibe las tramas binarias del XIAO
ESP32-C6 por UDP y sirve una interfaz local que se abre en el navegador.
"""

from __future__ import annotations

import argparse
from collections import deque
import json
import math
from pathlib import Path
import socket
import struct
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import threading
import time
from typing import Any, Protocol
from urllib.parse import urlsplit
import webbrowser

try:  # Permite ejecutar el archivo directamente o como modulo.
    from .protocol import (
        ALL_SENSOR_FLAGS,
        ChecksumError,
        FLAG_ENCODER_VALID,
        FLAG_OBSTACLE_ANGLE_VALID,
        FLAG_OBSTACLE_DISTANCE_VALID,
        FLAG_OBSTACLE_SEEN,
        FLAG_SPEED_VALID,
        FLAG_STANLEY_VALID,
        FLAG_STEERING_VALID,
        FLAG_TANGENTIAL_VALID,
        FLAG_VISION_FRESH,
        FRAME_SIZE,
        ProtocolError,
        TelemetryFrame,
        decode_frame,
        encode_frame,
    )
except ImportError:  # pragma: no cover - ruta usada por robot_debug.py directo
    from protocol import (
        ALL_SENSOR_FLAGS,
        ChecksumError,
        FLAG_ENCODER_VALID,
        FLAG_OBSTACLE_ANGLE_VALID,
        FLAG_OBSTACLE_DISTANCE_VALID,
        FLAG_OBSTACLE_SEEN,
        FLAG_SPEED_VALID,
        FLAG_STANLEY_VALID,
        FLAG_STEERING_VALID,
        FLAG_TANGENTIAL_VALID,
        FLAG_VISION_FRESH,
        FRAME_SIZE,
        ProtocolError,
        TelemetryFrame,
        decode_frame,
        encode_frame,
    )


DEFAULT_ROBOT_HOST = "192.168.4.1"
DEFAULT_UDP_PORT = 3333
DEFAULT_HTTP_PORT = 8765
SUBSCRIBE_MESSAGE = b"SUB1"
UNSUBSCRIBE_MESSAGE = b"UNS1"
ACK_MESSAGE = b"ACK1"
ACK_STATUS_SIZE = 36
_ACK_STATUS = struct.Struct("<4sBBH7I")


class BridgeStatus:
    """Contadores acumulados reportados por el puente XIAO."""

    __slots__ = (
        "valid_uart_frames",
        "uart_crc_errors",
        "envelope_errors",
        "coalesced_frames",
        "udp_send_errors",
        "udp_receive_errors",
        "uart_hardware_errors",
    )

    def __init__(
        self,
        valid_uart_frames: int,
        uart_crc_errors: int,
        envelope_errors: int,
        coalesced_frames: int,
        udp_send_errors: int,
        udp_receive_errors: int,
        uart_hardware_errors: int,
    ) -> None:
        self.valid_uart_frames = valid_uart_frames
        self.uart_crc_errors = uart_crc_errors
        self.envelope_errors = envelope_errors
        self.coalesced_frames = coalesced_frames
        self.udp_send_errors = udp_send_errors
        self.udp_receive_errors = udp_receive_errors
        self.uart_hardware_errors = uart_hardware_errors

    def as_dict(self) -> dict[str, int]:
        return {name: getattr(self, name) for name in self.__slots__}

    def __eq__(self, other: object) -> bool:
        return isinstance(other, BridgeStatus) and self.as_dict() == other.as_dict()


def parse_ack(payload: bytes) -> tuple[bool, BridgeStatus | None]:
    """Decodifica ACK corto/compatible/extendido del C6.

    Retorna ``(False, None)`` cuando el datagrama no es un ACK. Si empieza con
    ``ACK1`` pero su version o layout son incompatibles, lanza ``ProtocolError``.
    """

    if not payload.startswith(ACK_MESSAGE):
        return False, None
    if len(payload) == len(ACK_MESSAGE):
        return True, None
    if len(payload) < 6:
        raise ProtocolError(f"ACK1 truncado: {len(payload)} bytes")
    if payload[4] != 1 or payload[5] != FRAME_SIZE:
        raise ProtocolError("ACK1 incompatible: version o tamano de trama incorrectos")
    if len(payload) == 6:
        return True, None
    if len(payload) != ACK_STATUS_SIZE:
        raise ProtocolError(f"ACK1 extendido con tamano incorrecto: {len(payload)}")

    (
        _magic,
        _version,
        _telemetry_frame_size,
        status_size,
        valid_uart_frames,
        uart_crc_errors,
        envelope_errors,
        coalesced_frames,
        udp_send_errors,
        udp_receive_errors,
        uart_hardware_errors,
    ) = _ACK_STATUS.unpack(payload)
    if status_size != ACK_STATUS_SIZE:
        raise ProtocolError(f"ACK1 declara status_size incompatible: {status_size}")
    return True, BridgeStatus(
        valid_uart_frames=valid_uart_frames,
        uart_crc_errors=uart_crc_errors,
        envelope_errors=envelope_errors,
        coalesced_frames=coalesced_frames,
        udp_send_errors=udp_send_errors,
        udp_receive_errors=udp_receive_errors,
        uart_hardware_errors=uart_hardware_errors,
    )


def validate_ack(payload: bytes) -> bool:
    """Compatibilidad para consumidores que solo necesitan validar el ACK."""

    is_ack, _status = parse_ack(payload)
    return is_ack


class TelemetryStore:
    """Estado compartido sin colas: siempre conserva la muestra mas reciente."""

    def __init__(self, mode: str = "udp") -> None:
        self.mode = mode
        self._condition = threading.Condition(threading.RLock())
        self._latest: TelemetryFrame | None = None
        self._last_frame_at: float | None = None
        self._last_ack_at: float | None = None
        self._bridge_status: BridgeStatus | None = None
        self._arrival_times: deque[float] = deque(maxlen=4096)
        self._last_sequence: int | None = None
        self._last_teensy_time_us: int | None = None
        self._revision = 0
        self._stopped = False

        self.received_frames = 0
        self.lost_frames = 0
        self.duplicate_frames = 0
        self.out_of_order_frames = 0
        self.source_resets = 0
        self.crc_errors = 0
        self.malformed_frames = 0
        self.foreign_datagrams = 0
        self.transport_errors = 0
        self.last_error: str | None = None

    def _changed(self) -> None:
        self._revision += 1
        self._condition.notify_all()

    def push(self, frame: TelemetryFrame, received_at: float | None = None) -> bool:
        """Agrega una trama si es nueva. Retorna ``False`` para vieja/duplicada."""

        now = time.monotonic() if received_at is None else received_at
        with self._condition:
            if self._last_sequence is not None:
                delta = (frame.sequence - self._last_sequence) & 0xFFFFFFFF
                if delta == 0:
                    self.duplicate_frames += 1
                    self._changed()
                    return False
                if delta >= 0x80000000:
                    sequence_drop = self._last_sequence - frame.sequence
                    source_time_backwards = (
                        self._last_teensy_time_us is not None
                        and frame.teensy_time_us < self._last_teensy_time_us
                    )
                    quiet_gap = (
                        self._last_frame_at is not None
                        and now - self._last_frame_at >= 1.0
                    )
                    near_boot = (
                        frame.sequence <= 16
                        and frame.teensy_time_us <= 10_000_000
                    )
                    likely_restart = sequence_drop > 0 and (
                        quiet_gap
                        or near_boot
                        or (source_time_backwards and sequence_drop > 32)
                    )
                    if likely_restart:
                        self.source_resets += 1
                        self._arrival_times.clear()
                        delta = 1
                    else:
                        self.out_of_order_frames += 1
                        self._changed()
                        return False
                if delta > 1:
                    self.lost_frames += delta - 1

            self._last_sequence = frame.sequence
            self._last_teensy_time_us = frame.teensy_time_us
            self._latest = frame
            self._last_frame_at = now
            self._arrival_times.append(now)
            self.received_frames += 1
            self.last_error = None
            self._changed()
            return True

    def note_ack(
        self,
        bridge_status: BridgeStatus | None = None,
        received_at: float | None = None,
    ) -> None:
        now = time.monotonic() if received_at is None else received_at
        with self._condition:
            self._last_ack_at = now
            if bridge_status is not None:
                self._bridge_status = bridge_status
            self._changed()

    def note_protocol_error(self, error: Exception) -> None:
        with self._condition:
            if isinstance(error, ChecksumError):
                self.crc_errors += 1
            else:
                self.malformed_frames += 1
            self.last_error = str(error)
            self._changed()

    def note_foreign_datagram(self) -> None:
        with self._condition:
            self.foreign_datagrams += 1
            self._changed()

    def note_transport_error(self, message: str) -> None:
        with self._condition:
            self.transport_errors += 1
            self.last_error = message
            self._changed()

    def stop(self) -> None:
        with self._condition:
            self._stopped = True
            self._changed()

    def wait_for_change(self, revision: int, timeout: float) -> dict[str, Any]:
        with self._condition:
            self._condition.wait_for(
                lambda: self._revision != revision or self._stopped,
                timeout=timeout,
            )
            return self._snapshot_locked(time.monotonic())

    def snapshot(self) -> dict[str, Any]:
        with self._condition:
            return self._snapshot_locked(time.monotonic())

    def _snapshot_locked(self, now: float) -> dict[str, Any]:
        cutoff = now - 2.0
        while self._arrival_times and self._arrival_times[0] < cutoff:
            self._arrival_times.popleft()

        age_ms = (
            None
            if self._last_frame_at is None
            else max(0.0, (now - self._last_frame_at) * 1000.0)
        )
        ack_age_ms = (
            None
            if self._last_ack_at is None
            else max(0.0, (now - self._last_ack_at) * 1000.0)
        )
        connected = age_ms is not None and age_ms <= 2000.0
        c6_acknowledged = ack_age_ms is not None and ack_age_ms <= 2500.0

        if len(self._arrival_times) >= 2:
            span = self._arrival_times[-1] - self._arrival_times[0]
            rate_hz = (len(self._arrival_times) - 1) / span if span > 0 else 0.0
        else:
            rate_hz = 0.0
        if not connected:
            rate_hz = 0.0

        expected = self.received_frames + self.lost_frames
        loss_percent = (
            100.0 * self.lost_frames / expected if expected > 0 else 0.0
        )

        return {
            "revision": self._revision,
            "mode": self.mode,
            "telemetry": self._latest.as_dict() if self._latest else None,
            "bridge": (
                self._bridge_status.as_dict() if self._bridge_status else None
            ),
            "stats": {
                "connected": connected,
                "c6_acknowledged": c6_acknowledged,
                "rate_hz": rate_hz,
                "age_ms": age_ms,
                "ack_age_ms": ack_age_ms,
                "received_frames": self.received_frames,
                "lost_frames": self.lost_frames,
                "loss_percent": loss_percent,
                "duplicate_frames": self.duplicate_frames,
                "out_of_order_frames": self.out_of_order_frames,
                "source_resets": self.source_resets,
                "crc_errors": self.crc_errors,
                "malformed_frames": self.malformed_frames,
                "foreign_datagrams": self.foreign_datagrams,
                "transport_errors": self.transport_errors,
                "last_error": self.last_error,
            },
        }


class DataSource(Protocol):
    def start(self) -> None: ...

    def stop(self) -> None: ...


class UDPReceiver:
    """Cliente de suscripcion UDP para el servidor de telemetria del C6."""

    def __init__(self, store: TelemetryStore, robot_host: str, robot_port: int):
        self.store = store
        self.robot_host = robot_host
        self.robot_port = robot_port
        self._stop_event = threading.Event()
        self._thread: threading.Thread | None = None
        self._socket: socket.socket | None = None
        self._robot_ip: str | None = None

    def start(self) -> None:
        if self._thread is not None:
            return
        self._thread = threading.Thread(
            target=self._run, name="wro-udp-receiver", daemon=True
        )
        self._thread.start()

    def stop(self) -> None:
        self._stop_event.set()
        if self._thread is not None:
            self._thread.join(timeout=1.5)
        self._thread = None

    def _send(self, payload: bytes) -> None:
        if self._socket is not None and self._robot_ip is not None:
            self._socket.sendto(payload, (self._robot_ip, self.robot_port))

    def _run(self) -> None:
        try:
            self._robot_ip = socket.gethostbyname(self.robot_host)
        except OSError as error:
            self.store.note_transport_error(
                f"No se pudo resolver {self.robot_host}: {error}"
            )
            return

        udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket = udp_socket
        try:
            udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 256 * 1024)
            udp_socket.bind(("", 0))
            udp_socket.settimeout(0.10)
            next_subscription = 0.0

            while not self._stop_event.is_set():
                now = time.monotonic()
                if now >= next_subscription:
                    try:
                        self._send(SUBSCRIBE_MESSAGE)
                    except OSError as error:
                        self.store.note_transport_error(
                            f"No se pudo enviar SUB1: {error}"
                        )
                    next_subscription = now + 1.0

                try:
                    payload, address = udp_socket.recvfrom(2048)
                except socket.timeout:
                    continue
                except OSError as error:
                    if not self._stop_event.is_set():
                        self.store.note_transport_error(f"Error UDP: {error}")
                    break

                if address[0] != self._robot_ip or address[1] != self.robot_port:
                    self.store.note_foreign_datagram()
                    continue
                try:
                    is_ack, bridge_status = parse_ack(payload)
                    if is_ack:
                        self.store.note_ack(bridge_status)
                        continue
                except ProtocolError as error:
                    self.store.note_protocol_error(error)
                    continue

                try:
                    frame = decode_frame(payload)
                except ProtocolError as error:
                    self.store.note_protocol_error(error)
                    continue
                self.store.push(frame)
        except OSError as error:
            self.store.note_transport_error(f"No se pudo abrir UDP: {error}")
        finally:
            try:
                self._send(UNSUBSCRIBE_MESSAGE)
            except OSError:
                pass
            udp_socket.close()
            self._socket = None


class DemoPublisher:
    """Genera una carrera sintetica para probar toda la interfaz sin robot."""

    MM_PER_TICK = 0.13308

    def __init__(self, store: TelemetryStore, rate_hz: float = 100.0):
        if not 1.0 <= rate_hz <= 500.0:
            raise ValueError("demo-rate debe estar entre 1 y 500 Hz")
        self.store = store
        self.rate_hz = rate_hz
        self._stop_event = threading.Event()
        self._thread: threading.Thread | None = None

    def start(self) -> None:
        if self._thread is not None:
            return
        self._thread = threading.Thread(
            target=self._run, name="wro-demo-publisher", daemon=True
        )
        self._thread.start()

    def stop(self) -> None:
        self._stop_event.set()
        if self._thread is not None:
            self._thread.join(timeout=1.5)
        self._thread = None

    @staticmethod
    def _u16(value: float) -> int:
        return max(0, min(65535, round(value)))

    def _run(self) -> None:
        start = time.monotonic()
        deadline = start
        sequence = 0
        encoder_distance_mm = 0.0
        last_t = 0.0
        period = 1.0 / self.rate_hz

        while not self._stop_event.is_set():
            now = time.monotonic()
            if now < deadline:
                self._stop_event.wait(deadline - now)
                continue

            t = now - start
            delta_t = max(0.0, t - last_t)
            last_t = t
            speed_mm_s = 1050.0 + 260.0 * math.sin(t * 0.65)
            encoder_distance_mm += speed_mm_s * delta_t
            obstacle_seen = (t % 9.0) < 4.2
            obstacle_distance_mm = (
                350.0 + 700.0 * (0.5 + 0.5 * math.sin(t * 1.1))
                if obstacle_seen
                else math.nan
            )
            obstacle_angle_deg = (
                23.0 * math.sin(t * 1.7) if obstacle_seen else math.nan
            )
            stanley = 12.0 * math.sin(t * 0.9)
            tangential = (
                18.0 * math.sin(t * 1.35 + 0.7) if obstacle_seen else 0.0
            )
            controller_mode = 3 if obstacle_seen else 2
            steering = tangential if obstacle_seen else stanley

            flags = (
                FLAG_VISION_FRESH
                | FLAG_SPEED_VALID
                | FLAG_ENCODER_VALID
                | ALL_SENSOR_FLAGS
                | FLAG_STANLEY_VALID
                | FLAG_TANGENTIAL_VALID
                | FLAG_STEERING_VALID
            )
            if obstacle_seen:
                flags |= (
                    FLAG_OBSTACLE_SEEN
                    | FLAG_OBSTACLE_DISTANCE_VALID
                    | FLAG_OBSTACLE_ANGLE_VALID
                )

            frame = TelemetryFrame(
                flags=flags,
                sequence=sequence,
                teensy_time_us=int(t * 1_000_000.0) & 0xFFFFFFFF,
                speed_mm_s=speed_mm_s,
                lap=1 + int(t / 18.0) % 3,
                task=8 if obstacle_seen else 3,
                direction=1 if (int(t / 18.0) % 2 == 0) else -1,
                controller_mode=controller_mode,
                front_mm=self._u16(900.0 + 500.0 * math.sin(t * 0.45)),
                left_mm=self._u16(330.0 + 55.0 * math.sin(t * 1.0)),
                right_mm=self._u16(370.0 + 70.0 * math.sin(t * 0.82 + 1.0)),
                rear_mm=self._u16(1150.0 + 300.0 * math.sin(t * 0.33 + 2.0)),
                obstacle_distance_mm=obstacle_distance_mm,
                obstacle_angle_deg=obstacle_angle_deg,
                encoder_ticks=round(encoder_distance_mm / self.MM_PER_TICK),
                encoder_distance_mm=encoder_distance_mm,
                stanley_output_deg=stanley,
                tangential_output_deg=tangential,
                steering_command_deg=steering,
                motor_pwm=round(-125.0 + 18.0 * math.sin(t * 0.55)),
            )

            # El round-trip tambien ejercita el codec binario durante la demo.
            self.store.push(decode_frame(encode_frame(frame)), received_at=now)
            sequence = (sequence + 1) & 0xFFFFFFFF
            deadline += period
            if now - deadline > period * 4:
                deadline = now + period


class DashboardHTTPServer(ThreadingHTTPServer):
    daemon_threads = True
    allow_reuse_address = True

    def __init__(
        self,
        server_address: tuple[str, int],
        store: TelemetryStore,
        static_dir: Path,
    ) -> None:
        self.store = store
        self.static_dir = static_dir
        self.running = threading.Event()
        self.running.set()
        super().__init__(server_address, DashboardRequestHandler)

    def server_close(self) -> None:
        self.running.clear()
        self.store.stop()
        super().server_close()


class DashboardRequestHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"
    server: DashboardHTTPServer

    STATIC_FILES = {
        "/": ("index.html", "text/html; charset=utf-8"),
        "/index.html": ("index.html", "text/html; charset=utf-8"),
        "/app.js": ("app.js", "text/javascript; charset=utf-8"),
        "/style.css": ("style.css", "text/css; charset=utf-8"),
    }

    def log_message(self, _format: str, *_args: object) -> None:
        # Mantiene limpia la terminal; los errores de enlace estan en el dashboard.
        return

    def do_GET(self) -> None:  # noqa: N802 - API de BaseHTTPRequestHandler
        path = urlsplit(self.path).path
        if path == "/events":
            self._serve_events()
            return
        if path == "/api/snapshot":
            self._send_json(self.server.store.snapshot())
            return
        if path == "/favicon.ico":
            self.send_response(204)
            self.send_header("Content-Length", "0")
            self.end_headers()
            return
        static_entry = self.STATIC_FILES.get(path)
        if static_entry is None:
            self.send_error(404, "Recurso no encontrado")
            return
        filename, content_type = static_entry
        try:
            content = (self.server.static_dir / filename).read_bytes()
        except OSError:
            self.send_error(500, "No se pudo leer la interfaz")
            return
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(content)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()
        self.wfile.write(content)

    def _send_json(self, value: Any) -> None:
        content = json.dumps(
            value, ensure_ascii=False, allow_nan=False, separators=(",", ":")
        ).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(content)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()
        self.wfile.write(content)

    def _serve_events(self) -> None:
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream; charset=utf-8")
        self.send_header("Cache-Control", "no-cache, no-transform")
        self.send_header("Connection", "keep-alive")
        self.send_header("X-Accel-Buffering", "no")
        self.end_headers()

        revision = -1
        next_allowed_send = 0.0
        try:
            while self.server.running.is_set():
                snapshot = self.server.store.wait_for_change(revision, timeout=0.25)
                remaining = next_allowed_send - time.monotonic()
                if remaining > 0:
                    time.sleep(remaining)
                    if not self.server.running.is_set():
                        break
                # Se vuelve a leer despues de limitar a 50 Hz: asi se omiten
                # muestras viejas y siempre se entrega la mas reciente.
                snapshot = self.server.store.snapshot()
                revision = snapshot["revision"]
                payload = json.dumps(
                    snapshot,
                    ensure_ascii=False,
                    allow_nan=False,
                    separators=(",", ":"),
                ).encode("utf-8")
                self.wfile.write(b"event: telemetry\n")
                self.wfile.write(f"id: {revision}\n".encode("ascii"))
                self.wfile.write(b"data: " + payload + b"\n\n")
                self.wfile.flush()
                next_allowed_send = time.monotonic() + 0.020
        except (BrokenPipeError, ConnectionResetError, TimeoutError):
            pass
        finally:
            self.close_connection = True


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Visualiza en tiempo real la telemetria enviada por el XIAO ESP32-C6."
        )
    )
    parser.add_argument(
        "--robot",
        default=DEFAULT_ROBOT_HOST,
        help=f"IP o nombre del XIAO (predeterminado: {DEFAULT_ROBOT_HOST})",
    )
    parser.add_argument(
        "--udp-port",
        type=int,
        default=DEFAULT_UDP_PORT,
        help=f"puerto UDP del XIAO (predeterminado: {DEFAULT_UDP_PORT})",
    )
    parser.add_argument(
        "--http-port",
        type=int,
        default=DEFAULT_HTTP_PORT,
        help=f"puerto de la interfaz local (predeterminado: {DEFAULT_HTTP_PORT})",
    )
    parser.add_argument(
        "--no-browser",
        action="store_true",
        help="no abrir automaticamente el navegador",
    )
    parser.add_argument(
        "--demo", action="store_true", help="generar datos sin conectar el robot"
    )
    parser.add_argument(
        "--demo-rate",
        type=float,
        default=100.0,
        help="frecuencia del generador demo en Hz (predeterminado: 100)",
    )
    return parser


def run(args: argparse.Namespace) -> int:
    static_dir = Path(__file__).resolve().parent / "static"
    missing = [
        name
        for name in ("index.html", "app.js", "style.css")
        if not (static_dir / name).is_file()
    ]
    if missing:
        print(f"Faltan archivos de interfaz: {', '.join(missing)}")
        return 2

    mode = "demo" if args.demo else "udp"
    store = TelemetryStore(mode=mode)
    try:
        source: DataSource
        if args.demo:
            source = DemoPublisher(store, rate_hz=args.demo_rate)
        else:
            source = UDPReceiver(store, args.robot, args.udp_port)

        server = DashboardHTTPServer(("127.0.0.1", args.http_port), store, static_dir)
    except (OSError, ValueError) as error:
        print(f"No se pudo iniciar: {error}")
        return 2

    actual_port = server.server_address[1]
    url = f"http://127.0.0.1:{actual_port}/"
    source.start()
    if not args.no_browser:
        threading.Timer(0.25, lambda: webbrowser.open(url)).start()

    print("Monitor WRO activo")
    print(f"Interfaz: {url}")
    if args.demo:
        print(f"Modo demostracion: {args.demo_rate:g} Hz")
    else:
        print(f"XIAO: {args.robot}:{args.udp_port} (SUB1 cada segundo)")
    print("Presiona Ctrl+C para cerrar.")

    try:
        server.serve_forever(poll_interval=0.20)
    except KeyboardInterrupt:
        print("\nCerrando monitor...")
    finally:
        server.running.clear()
        source.stop()
        server.server_close()
    return 0


def main() -> int:
    return run(build_argument_parser().parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
