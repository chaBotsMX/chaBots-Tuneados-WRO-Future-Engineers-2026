"""Pruebas del protocolo binario y del estado de telemetria."""

from __future__ import annotations

from pathlib import Path
import struct
import sys
import time
import unittest


DEBUG_APP_DIR = Path(__file__).resolve().parents[1]
if str(DEBUG_APP_DIR) not in sys.path:
    sys.path.insert(0, str(DEBUG_APP_DIR))

from protocol import (  # noqa: E402
    ChecksumError,
    FLAG_ENCODER_VALID,
    FLAG_FRONT_VALID,
    FLAG_LEFT_VALID,
    FLAG_OBSTACLE_ANGLE_VALID,
    FLAG_OBSTACLE_DISTANCE_VALID,
    FLAG_OBSTACLE_SEEN,
    FLAG_REAR_VALID,
    FLAG_RIGHT_VALID,
    FLAG_SPEED_VALID,
    FLAG_STANLEY_VALID,
    FLAG_STEERING_VALID,
    FLAG_TANGENTIAL_VALID,
    FRAME_SIZE,
    MAGIC,
    ProtocolError,
    TelemetryFrame,
    VERSION,
    crc16_ccitt_false,
    decode_frame,
    encode_frame,
)
from robot_debug import TelemetryStore  # noqa: E402


def make_frame(**overrides: object) -> TelemetryFrame:
    values: dict[str, object] = {
        "flags": 0,
        "sequence": 42,
        "teensy_time_us": 0x10203040,
        "speed_mm_s": 123.5,
        "lap": 2,
        "task": 3,
        "direction": -1,
        "controller_mode": 4,
        "front_mm": 111,
        "left_mm": 222,
        "right_mm": 333,
        "rear_mm": 444,
        "obstacle_distance_mm": 555.25,
        "obstacle_angle_deg": -12.5,
        "encoder_ticks": -123456,
        "encoder_distance_mm": 678.5,
        "stanley_output_deg": -7.25,
        "tangential_output_deg": 8.5,
        "steering_command_deg": 1.75,
        "motor_pwm": -321,
    }
    values.update(overrides)
    return TelemetryFrame(**values)  # type: ignore[arg-type]


def repair_crc(payload: bytearray) -> bytes:
    struct.pack_into("<H", payload, 62, crc16_ccitt_false(payload[:62]))
    return bytes(payload)


class CrcTests(unittest.TestCase):
    def test_standard_check_vector(self) -> None:
        self.assertEqual(crc16_ccitt_false(b"123456789"), 0x29B1)


class FrameLayoutTests(unittest.TestCase):
    def test_exact_size_and_field_offsets(self) -> None:
        frame = make_frame(
            flags=0xA55A,
            sequence=0x12345678,
            teensy_time_us=0x90ABCDEF,
            speed_mm_s=123.5,
            lap=0x12,
            task=0x34,
            direction=-7,
            controller_mode=0x56,
            front_mm=0x1122,
            left_mm=0x3344,
            right_mm=0x5566,
            rear_mm=0x7788,
            obstacle_distance_mm=456.25,
            obstacle_angle_deg=-37.5,
            encoder_ticks=-123456789,
            encoder_distance_mm=789.5,
            stanley_output_deg=-12.25,
            tangential_output_deg=6.5,
            steering_command_deg=-5.75,
            motor_pwm=-321,
        )
        payload = encode_frame(frame)

        self.assertEqual(FRAME_SIZE, 64)
        self.assertEqual(len(payload), 64)

        expected_fields = (
            ("magic", 0, "<4s", MAGIC),
            ("version", 4, "<B", VERSION),
            ("frame_size", 5, "<B", 64),
            ("flags", 6, "<H", 0xA55A),
            ("sequence", 8, "<I", 0x12345678),
            ("teensy_time_us", 12, "<I", 0x90ABCDEF),
            ("speed_mm_s", 16, "<f", 123.5),
            ("lap", 20, "<B", 0x12),
            ("task", 21, "<B", 0x34),
            ("direction", 22, "<b", -7),
            ("controller_mode", 23, "<B", 0x56),
            ("front_mm", 24, "<H", 0x1122),
            ("left_mm", 26, "<H", 0x3344),
            ("right_mm", 28, "<H", 0x5566),
            ("rear_mm", 30, "<H", 0x7788),
            ("obstacle_distance_mm", 32, "<f", 456.25),
            ("obstacle_angle_deg", 36, "<f", -37.5),
            ("encoder_ticks", 40, "<i", -123456789),
            ("encoder_distance_mm", 44, "<f", 789.5),
            ("stanley_output_deg", 48, "<f", -12.25),
            ("tangential_output_deg", 52, "<f", 6.5),
            ("steering_command_deg", 56, "<f", -5.75),
            ("motor_pwm", 60, "<h", -321),
        )
        for name, offset, field_format, expected in expected_fields:
            with self.subTest(field=name, offset=offset):
                actual = struct.unpack_from(field_format, payload, offset)[0]
                self.assertEqual(actual, expected)

        received_crc = struct.unpack_from("<H", payload, 62)[0]
        self.assertEqual(received_crc, crc16_ccitt_false(payload[:62]))

    def test_encode_decode_roundtrip(self) -> None:
        validity_flags = (
            FLAG_OBSTACLE_SEEN
            | FLAG_SPEED_VALID
            | FLAG_ENCODER_VALID
            | FLAG_FRONT_VALID
            | FLAG_LEFT_VALID
            | FLAG_RIGHT_VALID
            | FLAG_REAR_VALID
            | FLAG_OBSTACLE_DISTANCE_VALID
            | FLAG_OBSTACLE_ANGLE_VALID
            | FLAG_STANLEY_VALID
            | FLAG_TANGENTIAL_VALID
            | FLAG_STEERING_VALID
        )
        frame = make_frame(flags=validity_flags, sequence=0xFFFFFFFF)

        self.assertEqual(decode_frame(encode_frame(frame)), frame)


class FrameValidationTests(unittest.TestCase):
    def test_corrupt_payload_rejects_crc(self) -> None:
        payload = bytearray(encode_frame(make_frame()))
        payload[16] ^= 0x01

        with self.assertRaises(ChecksumError):
            decode_frame(payload)

    def test_invalid_magic_version_and_declared_size(self) -> None:
        valid = encode_frame(make_frame())
        cases = (
            ("magic", 0, ord("X"), "magic"),
            ("version", 4, VERSION + 1, "version"),
            ("frame_size", 5, FRAME_SIZE - 1, "frame_size"),
        )

        for name, offset, replacement, message in cases:
            with self.subTest(case=name):
                payload = bytearray(valid)
                payload[offset] = replacement
                with self.assertRaisesRegex(ProtocolError, message):
                    decode_frame(repair_crc(payload))

    def test_invalid_physical_size(self) -> None:
        valid = encode_frame(make_frame())

        for payload in (valid[:-1], valid + b"\x00"):
            with self.subTest(size=len(payload)):
                with self.assertRaisesRegex(ProtocolError, "tamano incorrecto"):
                    decode_frame(payload)

    def test_invalid_fields_become_none_according_to_flags(self) -> None:
        data = make_frame(flags=FLAG_OBSTACLE_SEEN).as_dict()

        invalid_fields = (
            "speed_mm_s",
            "front_mm",
            "left_mm",
            "right_mm",
            "rear_mm",
            "obstacle_distance_mm",
            "obstacle_angle_deg",
            "encoder_ticks",
            "encoder_distance_mm",
            "stanley_output_deg",
            "tangential_output_deg",
            "steering_command_deg",
        )
        for field in invalid_fields:
            with self.subTest(field=field):
                self.assertIsNone(data[field])
        self.assertTrue(data["obstacle_seen"])


class TelemetryStoreSequenceTests(unittest.TestCase):
    def test_loss_duplicate_and_out_of_order_statistics(self) -> None:
        store = TelemetryStore(mode="test")
        now = time.monotonic()

        self.assertTrue(store.push(make_frame(sequence=10), now))
        self.assertTrue(store.push(make_frame(sequence=13), now + 0.01))
        self.assertFalse(store.push(make_frame(sequence=13), now + 0.02))
        self.assertFalse(store.push(make_frame(sequence=12), now + 0.03))

        snapshot = store.snapshot()
        stats = snapshot["stats"]
        self.assertEqual(stats["received_frames"], 2)
        self.assertEqual(stats["lost_frames"], 2)
        self.assertEqual(stats["duplicate_frames"], 1)
        self.assertEqual(stats["out_of_order_frames"], 1)
        self.assertAlmostEqual(stats["loss_percent"], 50.0)
        self.assertEqual(snapshot["telemetry"]["sequence"], 13)

    def test_consecutive_sequence_wrap_is_in_order(self) -> None:
        store = TelemetryStore(mode="test")
        now = time.monotonic()

        for index, sequence in enumerate((0xFFFFFFFE, 0xFFFFFFFF, 0, 1)):
            with self.subTest(sequence=sequence):
                self.assertTrue(
                    store.push(make_frame(sequence=sequence), now + index * 0.01)
                )

        snapshot = store.snapshot()
        stats = snapshot["stats"]
        self.assertEqual(stats["received_frames"], 4)
        self.assertEqual(stats["lost_frames"], 0)
        self.assertEqual(stats["duplicate_frames"], 0)
        self.assertEqual(stats["out_of_order_frames"], 0)
        self.assertEqual(snapshot["telemetry"]["sequence"], 1)

    def test_loss_across_sequence_wrap_is_counted(self) -> None:
        store = TelemetryStore(mode="test")
        now = time.monotonic()

        self.assertTrue(store.push(make_frame(sequence=0xFFFFFFFE), now))
        self.assertTrue(store.push(make_frame(sequence=1), now + 0.01))

        stats = store.snapshot()["stats"]
        self.assertEqual(stats["received_frames"], 2)
        self.assertEqual(stats["lost_frames"], 2)
        self.assertEqual(stats["out_of_order_frames"], 0)

    def test_source_restart_creates_a_new_sequence_baseline(self) -> None:
        store = TelemetryStore(mode="test")
        now = time.monotonic()

        self.assertTrue(
            store.push(
                make_frame(sequence=5000, teensy_time_us=600_000_000), now
            )
        )
        self.assertTrue(
            store.push(make_frame(sequence=0, teensy_time_us=1000), now + 0.01)
        )
        self.assertTrue(
            store.push(make_frame(sequence=1, teensy_time_us=2000), now + 0.02)
        )

        snapshot = store.snapshot()
        stats = snapshot["stats"]
        self.assertEqual(stats["received_frames"], 3)
        self.assertEqual(stats["source_resets"], 1)
        self.assertEqual(stats["lost_frames"], 0)
        self.assertEqual(stats["out_of_order_frames"], 0)
        self.assertEqual(snapshot["telemetry"]["sequence"], 1)

    def test_nearby_old_packet_is_out_of_order_not_a_restart(self) -> None:
        store = TelemetryStore(mode="test")
        now = time.monotonic()

        self.assertTrue(
            store.push(
                make_frame(sequence=100, teensy_time_us=1_000_000), now
            )
        )
        self.assertFalse(
            store.push(
                make_frame(sequence=99, teensy_time_us=999_000), now + 0.01
            )
        )

        snapshot = store.snapshot()
        stats = snapshot["stats"]
        self.assertEqual(stats["received_frames"], 1)
        self.assertEqual(stats["source_resets"], 0)
        self.assertEqual(stats["out_of_order_frames"], 1)
        self.assertEqual(snapshot["telemetry"]["sequence"], 100)


if __name__ == "__main__":
    unittest.main()
