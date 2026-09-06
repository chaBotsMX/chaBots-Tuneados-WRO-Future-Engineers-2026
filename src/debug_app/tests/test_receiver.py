"""Pruebas de los mensajes de control UDP del XIAO."""

from __future__ import annotations

from pathlib import Path
import struct
import sys
import unittest


DEBUG_APP_DIR = Path(__file__).resolve().parents[1]
if str(DEBUG_APP_DIR) not in sys.path:
    sys.path.insert(0, str(DEBUG_APP_DIR))

from protocol import ProtocolError  # noqa: E402
from robot_debug import TelemetryStore, parse_ack, validate_ack  # noqa: E402


class AcknowledgementTests(unittest.TestCase):
    def test_accepts_short_and_extended_v1_ack(self) -> None:
        self.assertTrue(validate_ack(b"ACK1"))
        self.assertTrue(validate_ack(b"ACK1\x01\x40"))

    def test_decodes_36_byte_bridge_status(self) -> None:
        payload = struct.pack(
            "<4sBBH7I",
            b"ACK1",
            1,
            64,
            36,
            1000,
            2,
            3,
            400,
            5,
            6,
            7,
        )
        is_ack, bridge = parse_ack(payload)

        self.assertTrue(is_ack)
        self.assertIsNotNone(bridge)
        self.assertEqual(
            bridge.as_dict(),
            {
                "valid_uart_frames": 1000,
                "uart_crc_errors": 2,
                "envelope_errors": 3,
                "coalesced_frames": 400,
                "udp_send_errors": 5,
                "udp_receive_errors": 6,
                "uart_hardware_errors": 7,
            },
        )

        store = TelemetryStore()
        store.note_ack(bridge)
        self.assertEqual(store.snapshot()["bridge"], bridge.as_dict())

    def test_non_ack_is_ignored(self) -> None:
        self.assertFalse(validate_ack(b"WRO1"))

    def test_incompatible_ack_is_rejected(self) -> None:
        for payload in (
            b"ACK1\x01",
            b"ACK1\x02\x40",
            b"ACK1\x01\x3f",
            b"ACK1\x01\x40extra",
            struct.pack("<4sBBH7I", b"ACK1", 1, 64, 35, *range(7)),
        ):
            with self.subTest(payload=payload):
                with self.assertRaises(ProtocolError):
                    validate_ack(payload)


if __name__ == "__main__":
    unittest.main()
