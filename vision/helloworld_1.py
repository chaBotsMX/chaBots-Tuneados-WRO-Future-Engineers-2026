import csi
import time
from machine import UART


OBSTACLE_THRESHOLDS = [
    (12, 60, 30, 98, -25, 50),
    (23, 85, -128, -30, 2, 127)   # Verde
]

PARKING_WALL_THRESHOLDS = [
    (10, 56, 14, 86, -128, 9)     # Magenta
]
BLUE_LINE_THRESHOLD = [
    (0, 70, -30, 30, -128, -10),
]

UART_ID = 3
UART_BAUDRATE = 115200

START_BYTE_HIGH = 0xAA
START_BYTE_LOW = 0x55
PACKET_SIZE = 14

NOT_FOUND_X = 250
NOT_FOUND_Y = 250

DETECTION_ROI = (0, 0, 320, 120)

MIN_OBSTACLE_PIXELS = 100
MIN_OBSTACLE_AREA = 60
MIN_WALL_PIXELS = 100
MIN_WALL_AREA = 100
MIN_LINE_PIXELS = 80
MIN_LINE_AREA = 40

MIN_OBSTACLE_ASPECT_RATIO = 0.5
MIN_WALL_ASPECT_RATIO = 1.0

PRINT_DIAGNOSTICS = False


def is_vertical_obstacle(blob):
    is_vertical = blob.h > blob.w * MIN_OBSTACLE_ASPECT_RATIO
    if PRINT_DIAGNOSTICS:
        print("vertical obstacle:", is_vertical)
    return is_vertical


def is_horizontal_parking_wall(blob):
    return blob.w > blob.h * MIN_WALL_ASPECT_RATIO


def largest_valid_blob(blobs, validator=None):
    largest = None

    for blob in blobs:
        if validator is not None and not validator(blob):
            continue

        if largest is None or blob.pixels > largest.pixels:
            largest = blob

    return largest


def put_uint16_be(buffer, offset, value):
    value = max(0, min(0xFFFF, int(value)))
    buffer[offset] = (value >> 8) & 0xFF
    buffer[offset + 1] = value & 0xFF


def update_packet(
    packet,
    obstacle_position,
    wall_position,
    blue_line_bottom_y,
    flags,
):
    packet[0] = START_BYTE_HIGH
    packet[1] = START_BYTE_LOW

    put_uint16_be(packet, 2, obstacle_position[0])
    put_uint16_be(packet, 4, obstacle_position[1])
    put_uint16_be(packet, 6, wall_position[0])
    put_uint16_be(packet, 8, wall_position[1])
    put_uint16_be(packet, 10, blue_line_bottom_y)
    packet[12] = flags & 0xFF

    checksum = 0
    for index in range(2, 13):
        checksum ^= packet[index]
    packet[13] = checksum


def configure_camera():
    camera = csi.CSI()
    camera.reset()
    camera.pixformat(csi.RGB565)
    camera.framesize(csi.QVGA)

    camera.auto_blc(False, regs=[129, 132, 134, 133, 128, 132, 120, 128])
    camera.auto_gain(False, gain_db=8.0)
    camera.auto_exposure(False, exposure_us=3000)
    camera.auto_whitebal(False, rgb_gain_db=(3.0, 3.0, 6.0))
    camera.brightness(-1)
    camera.saturation(0)
    camera.contrast(-1)

    if camera.cid() in (csi.OV7725, csi.OV5640):
        camera.ioctl(csi.IOCTL_SET_NIGHT_MODE, False)

    camera.snapshot(time=2000)
    return camera


def main():
    camera = configure_camera()
    uart = UART(UART_ID, baudrate=UART_BAUDRATE)
    packet = bytearray(PACKET_SIZE)
    clock = time.clock()

    while True:
        clock.tick()
        img = camera.snapshot()

        obstacle_blobs = img.find_blobs(
            OBSTACLE_THRESHOLDS,
            roi=DETECTION_ROI,
            pixels_threshold=MIN_OBSTACLE_PIXELS,
            area_threshold=MIN_OBSTACLE_AREA,
            merge=False,
        )

        wall_blobs = img.find_blobs(
            PARKING_WALL_THRESHOLDS,
            roi=DETECTION_ROI,
            pixels_threshold=MIN_WALL_PIXELS,
            area_threshold=MIN_WALL_AREA,
            merge=True,
        )

        line_blobs = img.find_blobs(
            BLUE_LINE_THRESHOLD,
            roi=DETECTION_ROI,
            pixels_threshold=MIN_LINE_PIXELS,
            area_threshold=MIN_LINE_AREA,
            merge=True,
        )
        line = largest_valid_blob(line_blobs, lambda blob: True)
        obstacle = largest_valid_blob(obstacle_blobs, is_vertical_obstacle)
        parking_wall = largest_valid_blob(
            wall_blobs,
            is_horizontal_parking_wall,
        )

        obstacle_position = [NOT_FOUND_X, NOT_FOUND_Y]
        wall_position = [NOT_FOUND_X, NOT_FOUND_Y]
        blue_line_bottom_y = NOT_FOUND_Y
        flags = 0

        if line is not None:
            blue_line_bottom_y = min(
                239,
                int(line.y + line.h - 1),
            )
            img.draw_rectangle(
                line.rect,
                color=(0, 0, 255),
                thickness=2,
            )
            img.draw_cross(
                (line.cx, blue_line_bottom_y),
                color=(0, 0, 255),
                thickness=2,
            )

        if obstacle is not None:
            obstacle_floor_y = max(
                0,
                int(obstacle.cy - obstacle.h / 2),
            )
            obstacle_position[0] = obstacle.cx
            obstacle_position[1] = obstacle_floor_y
            flags |= 0x01
            flags |= (obstacle.code & 0x03) << 2

            img.draw_rectangle(
                obstacle.rect,
                color=(0, 255, 0),
                thickness=2,
            )
            img.draw_cross(
                (obstacle.cx, obstacle_floor_y),
                color=(0, 255, 0),
                thickness=2,
            )
            img.draw_string(
                (obstacle.x, max(0, obstacle.y - 12)),
                "OBSTACLE",
                color=(0, 255, 0),
            )

        if parking_wall is not None:
            wall_position[0] = parking_wall.cx
            wall_position[1] = parking_wall.cy
            flags |= 0x02

            img.draw_rectangle(
                parking_wall.rect,
                color=(255, 0, 255),
                thickness=2,
            )
            img.draw_cross(
                (parking_wall.cx, parking_wall.cy),
                color=(255, 0, 255),
                thickness=2,
            )
            img.draw_string(
                (parking_wall.x, max(0, parking_wall.y - 12)),
                "PARKING WALL",
                color=(255, 0, 255),
            )

        img.draw_rectangle(
            DETECTION_ROI,
            color=(255, 0, 0),
            thickness=2,
        )

        update_packet(
            packet,
            obstacle_position,
            wall_position,
            blue_line_bottom_y,
            flags,
        )
        uart.write(packet)

        if PRINT_DIAGNOSTICS:
            print(
                "obstacle:",
                obstacle_position,
                "wall:",
                wall_position,
                "blue line bottom y:",
                blue_line_bottom_y,
                "fps:",
                clock.fps(),
            )


main()
