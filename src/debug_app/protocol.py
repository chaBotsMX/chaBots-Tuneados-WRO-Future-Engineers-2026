"""Protocolo binario de telemetria WRO v1.

La trama es deliberadamente fija: 64 bytes, little-endian y CRC16-CCITT-FALSE.
El XIAO puede reenviar el mismo bloque recibido por UART como un datagrama UDP sin
transformarlo. Este modulo es independiente de la interfaz para poder probarlo y
reutilizarlo en herramientas de captura.
"""

from __future__ import annotations

from dataclasses import dataclass
import math
import struct
from typing import Any


MAGIC = b"WRO1"
VERSION = 1
FRAME_SIZE = 64

# Los espacios se omiten intencionalmente: cada caracter corresponde al orden
# documentado en README.md y al layout que usara la Teensy.
_FRAME_WITHOUT_CRC = struct.Struct("<4sBBHIIfBBbBHHHHffiffffh")
_FRAME = struct.Struct("<4sBBHIIfBBbBHHHHffiffffhH")

assert _FRAME_WITHOUT_CRC.size == 62
assert _FRAME.size == FRAME_SIZE


FLAG_OBSTACLE_SEEN = 1 << 0
FLAG_VISION_FRESH = 1 << 1
FLAG_SPEED_VALID = 1 << 2
FLAG_ENCODER_VALID = 1 << 3
FLAG_FRONT_VALID = 1 << 4
FLAG_LEFT_VALID = 1 << 5
FLAG_RIGHT_VALID = 1 << 6
FLAG_REAR_VALID = 1 << 7
FLAG_OBSTACLE_DISTANCE_VALID = 1 << 8
FLAG_OBSTACLE_ANGLE_VALID = 1 << 9
FLAG_STANLEY_VALID = 1 << 10
FLAG_TANGENTIAL_VALID = 1 << 11
FLAG_STEERING_VALID = 1 << 12
FLAG_TEENSY_TX_DROPPED = 1 << 13

ALL_SENSOR_FLAGS = (
    FLAG_FRONT_VALID | FLAG_LEFT_VALID | FLAG_RIGHT_VALID | FLAG_REAR_VALID
)


class ProtocolError(ValueError):
    """Trama incompatible o mal formada."""


class ChecksumError(ProtocolError):
    """El CRC recibido no coincide con el contenido de la trama."""


def crc16_ccitt_false(data: bytes | bytearray | memoryview) -> int:
    """Calcula CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF)."""

    crc = 0xFFFF
    for value in data:
        crc ^= value << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def _valid_float(value: float, valid: bool) -> float | None:
    if not valid or not math.isfinite(value):
        return None
    return value


def _valid_int(value: int, valid: bool) -> int | None:
    return value if valid else None


@dataclass(frozen=True, slots=True)
class TelemetryFrame:
    flags: int
    sequence: int
    teensy_time_us: int
    speed_mm_s: float
    lap: int
    task: int
    direction: int
    controller_mode: int
    front_mm: int
    left_mm: int
    right_mm: int
    rear_mm: int
    obstacle_distance_mm: float
    obstacle_angle_deg: float
    encoder_ticks: int
    encoder_distance_mm: float
    stanley_output_deg: float
    tangential_output_deg: float
    steering_command_deg: float
    motor_pwm: int

    def has(self, flag: int) -> bool:
        return bool(self.flags & flag)

    def as_dict(self) -> dict[str, Any]:
        """Representacion JSON segura; los datos invalidos se vuelven ``None``."""

        encoder_valid = self.has(FLAG_ENCODER_VALID)
        return {
            "protocol_version": VERSION,
            "flags": self.flags,
            "sequence": self.sequence,
            "teensy_time_us": self.teensy_time_us,
            "speed_mm_s": _valid_float(
                self.speed_mm_s, self.has(FLAG_SPEED_VALID)
            ),
            "lap": self.lap,
            "task": self.task,
            "direction": self.direction,
            "controller_mode": self.controller_mode,
            "front_mm": _valid_int(
                self.front_mm, self.has(FLAG_FRONT_VALID)
            ),
            "left_mm": _valid_int(self.left_mm, self.has(FLAG_LEFT_VALID)),
            "right_mm": _valid_int(
                self.right_mm, self.has(FLAG_RIGHT_VALID)
            ),
            "rear_mm": _valid_int(self.rear_mm, self.has(FLAG_REAR_VALID)),
            "obstacle_seen": self.has(FLAG_OBSTACLE_SEEN),
            "vision_fresh": self.has(FLAG_VISION_FRESH),
            "obstacle_distance_mm": _valid_float(
                self.obstacle_distance_mm,
                self.has(FLAG_OBSTACLE_DISTANCE_VALID),
            ),
            "obstacle_angle_deg": _valid_float(
                self.obstacle_angle_deg,
                self.has(FLAG_OBSTACLE_ANGLE_VALID),
            ),
            "encoder_ticks": _valid_int(self.encoder_ticks, encoder_valid),
            "encoder_distance_mm": _valid_float(
                self.encoder_distance_mm, encoder_valid
            ),
            "stanley_output_deg": _valid_float(
                self.stanley_output_deg, self.has(FLAG_STANLEY_VALID)
            ),
            "tangential_output_deg": _valid_float(
                self.tangential_output_deg,
                self.has(FLAG_TANGENTIAL_VALID),
            ),
            "steering_command_deg": _valid_float(
                self.steering_command_deg, self.has(FLAG_STEERING_VALID)
            ),
            "motor_pwm": self.motor_pwm,
            "teensy_tx_dropped": self.has(FLAG_TEENSY_TX_DROPPED),
        }


def decode_frame(data: bytes | bytearray | memoryview) -> TelemetryFrame:
    """Valida y decodifica exactamente una trama WRO v1."""

    if len(data) != FRAME_SIZE:
        raise ProtocolError(
            f"tamano incorrecto: {len(data)} bytes; se esperaban {FRAME_SIZE}"
        )

    view = memoryview(data)
    if bytes(view[0:4]) != MAGIC:
        raise ProtocolError("magic incorrecto")
    if view[4] != VERSION:
        raise ProtocolError(f"version no soportada: {view[4]}")
    if view[5] != FRAME_SIZE:
        raise ProtocolError(f"frame_size declarado incorrecto: {view[5]}")

    received_crc = struct.unpack_from("<H", view, 62)[0]
    calculated_crc = crc16_ccitt_false(view[:62])
    if received_crc != calculated_crc:
        raise ChecksumError(
            f"CRC incorrecto: recibido 0x{received_crc:04X}, "
            f"calculado 0x{calculated_crc:04X}"
        )

    (
        _magic,
        _version,
        _frame_size,
        flags,
        sequence,
        teensy_time_us,
        speed_mm_s,
        lap,
        task,
        direction,
        controller_mode,
        front_mm,
        left_mm,
        right_mm,
        rear_mm,
        obstacle_distance_mm,
        obstacle_angle_deg,
        encoder_ticks,
        encoder_distance_mm,
        stanley_output_deg,
        tangential_output_deg,
        steering_command_deg,
        motor_pwm,
        _crc,
    ) = _FRAME.unpack(view)

    return TelemetryFrame(
        flags=flags,
        sequence=sequence,
        teensy_time_us=teensy_time_us,
        speed_mm_s=speed_mm_s,
        lap=lap,
        task=task,
        direction=direction,
        controller_mode=controller_mode,
        front_mm=front_mm,
        left_mm=left_mm,
        right_mm=right_mm,
        rear_mm=rear_mm,
        obstacle_distance_mm=obstacle_distance_mm,
        obstacle_angle_deg=obstacle_angle_deg,
        encoder_ticks=encoder_ticks,
        encoder_distance_mm=encoder_distance_mm,
        stanley_output_deg=stanley_output_deg,
        tangential_output_deg=tangential_output_deg,
        steering_command_deg=steering_command_deg,
        motor_pwm=motor_pwm,
    )


def encode_frame(frame: TelemetryFrame) -> bytes:
    """Codifica una trama; se usa principalmente por el modo demo y las pruebas."""

    prefix = _FRAME_WITHOUT_CRC.pack(
        MAGIC,
        VERSION,
        FRAME_SIZE,
        frame.flags,
        frame.sequence,
        frame.teensy_time_us,
        frame.speed_mm_s,
        frame.lap,
        frame.task,
        frame.direction,
        frame.controller_mode,
        frame.front_mm,
        frame.left_mm,
        frame.right_mm,
        frame.rear_mm,
        frame.obstacle_distance_mm,
        frame.obstacle_angle_deg,
        frame.encoder_ticks,
        frame.encoder_distance_mm,
        frame.stanley_output_deg,
        frame.tangential_output_deg,
        frame.steering_command_deg,
        frame.motor_pwm,
    )
    return prefix + struct.pack("<H", crc16_ccitt_false(prefix))


__all__ = [
    "ALL_SENSOR_FLAGS",
    "ChecksumError",
    "FLAG_ENCODER_VALID",
    "FLAG_FRONT_VALID",
    "FLAG_LEFT_VALID",
    "FLAG_OBSTACLE_ANGLE_VALID",
    "FLAG_OBSTACLE_DISTANCE_VALID",
    "FLAG_OBSTACLE_SEEN",
    "FLAG_REAR_VALID",
    "FLAG_RIGHT_VALID",
    "FLAG_SPEED_VALID",
    "FLAG_STANLEY_VALID",
    "FLAG_STEERING_VALID",
    "FLAG_TANGENTIAL_VALID",
    "FLAG_TEENSY_TX_DROPPED",
    "FLAG_VISION_FRESH",
    "FRAME_SIZE",
    "MAGIC",
    "ProtocolError",
    "TelemetryFrame",
    "VERSION",
    "crc16_ccitt_false",
    "decode_frame",
    "encode_frame",
]
