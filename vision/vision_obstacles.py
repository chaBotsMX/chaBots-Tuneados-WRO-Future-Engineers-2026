import csi
import math
import pyb
import time
from machine import UART


OBSTACLE_THRESHOLDS = [
    (0, 32, 22, 127, -128, 116),  # Red
    (0, 49, -128, -12, 2, 127)  # Green
]

PARKING_WALL_THRESHOLDS = [
    (0, 34, 13, 127, -128, 4)    # Magenta
]

BLUE_LINE_THRESHOLD = [
    (0, 79, -128, 20, -128, -7),  # Blue
]

BLACK_WALL_THRESHOLD = [(0, 21, -128, 38, -9, 127)]  # Calibrate for black walls
BLACK_WALL_ROI = (0, 30, 320, 100)  # Nearby wall region; project toward the pillars
WALL_FLOOR_REFERENCE = (160, 4)  # Nearby floor point in crop-local coordinates
MIN_WALL_LINE_LENGTH = 25
MIN_WALL_LINE_HEIGHT = 18
WALL_SAMPLE_DISTANCE = 4
WALL_BOUNDARY_MARGIN = 6  # Pixel tolerance, not distance from the robot
WALL_HOLD_MS = 1000  # Keep each wall after losing it; 0 disables retention

UART_ID = 3
UART_BAUDRATE = 115200

START_BYTE_HIGH = 0xAA
START_BYTE_LOW = 0x55
PACKET_SIZE = 14

NOT_FOUND_X = 250
NOT_FOUND_Y = 250

DETECTION_ROI = (20, 30, 280, 80)
BLUE_DETECTION_ROI = (0, 12, 320, 38)

MIN_OBSTACLE_PIXELS = 40
MIN_OBSTACLE_AREA = 40
MIN_WALL_PIXELS = 100
MIN_WALL_AREA = 100
MIN_LINE_PIXELS = 120
MIN_LINE_AREA = 40

MIN_OBSTACLE_ASPECT_RATIO = 0.5
MIN_WALL_ASPECT_RATIO = 1.0
OBSTACLE_HOLD_MS = 150  # Wait after losing the target before locking onto another
OBSTACLE_MATCH_DISTANCE = 40  # Maximum base displacement in pixels

PRINT_DIAGNOSTICS = False


def is_vertical_obstacle(blob):
    is_vertical = blob.h > blob.w * MIN_OBSTACLE_ASPECT_RATIO
    if PRINT_DIAGNOSTICS:
        print("vertical obstacle:", is_vertical)
    return is_vertical


def is_horizontal_parking_wall(blob):
    return blob.w > blob.h * MIN_WALL_ASPECT_RATIO


def find_wall_boundaries(img, color_blobs):
    # White mask pixels represent black walls. Leave the original image unchanged.
    roi_x, roi_y = BLACK_WALL_ROI[0], BLACK_WALL_ROI[1]
    mask = img.copy(roi=BLACK_WALL_ROI)
    mask.binary(BLACK_WALL_THRESHOLD)
    mask.to_grayscale()
    reference_x, reference_y = WALL_FLOOR_REFERENCE
    boundaries = [None, None]
    scores = [0, 0]

    def is_wall(x, y):
        x, y = int(round(x)), int(round(y))
        if not (0 <= x < BLACK_WALL_ROI[2] and 0 <= y < BLACK_WALL_ROI[3]):
            return None
        for blob in color_blobs:
            if (blob.x <= x + roi_x < blob.x + blob.w and
                    blob.y <= y + roi_y < blob.y + blob.h):
                return None
        return mask.get_pixel((x, y)) > 127

    if is_wall(reference_x, reference_y) is not False:
        return []

    for line in mask.find_line_segments(merge_distance=5, max_theta_diff=10):
        x1, y1, x2, y2 = line[0], line[1], line[2], line[3]
        dx, dy = x2 - x1, y2 - y1
        length = math.sqrt(dx * dx + dy * dy)
        if length < MIN_WALL_LINE_LENGTH or abs(dy) < MIN_WALL_LINE_HEIGHT:
            continue

        # a*x + b*y + c is signed distance; positive toward our floor region.
        a, b = -dy / length, dx / length
        c = -a * x1 - b * y1
        reference_distance = a * reference_x + b * reference_y + c
        if abs(reference_distance) <= WALL_BOUNDARY_MARGIN:
            continue
        if reference_distance < 0:
            a, b, c = -a, -b, -c

        valid_samples = 0
        for index in range(1, 6):
            x = x1 + dx * index / 6
            y = y1 + dy * index / 6
            distance = WALL_SAMPLE_DISTANCE
            if is_wall(x - a * distance, y - b * distance) is not True:
                continue
            if is_wall(x + a * distance, y + b * distance) is not False:
                continue
            # Require continuous floor toward the robot to exclude the wall's top edge.
            if all(is_wall(x + (reference_x - x) * step / 5,
                           y + (reference_y - y) * step / 5) is False
                   for step in range(1, 5)):
                valid_samples += 1

        if valid_samples < 4:
            continue

        near_x = x1 + dx * (reference_y - y1) / dy
        side = 0 if near_x < reference_x else 1
        # Favor long nearby edges over the wall face beyond the corner.
        score = length / (1 + max(0, min(y1, y2)))
        if score > scores[side]:
            scores[side] = score
            # Convert the line from crop-local to full-image coordinates.
            boundaries[side] = (a, b, c - a * roi_x - b * roi_y)

    return [boundary for boundary in boundaries if boundary is not None]


def is_obstacle_inside_walls(blob, boundaries):
    if not is_vertical_obstacle(blob):
        return False
    # Reject the entire blob if a projected line crosses or touches its rectangle.
    for a, b, c in boundaries:
        distances = [a * x + b * y + c
                     for x in (blob.x, blob.x + blob.w - 1)
                     for y in (blob.y, blob.y + blob.h - 1)]
        if min(distances) <= 0 <= max(distances):
            return False
    # With the camera rotated 180 degrees, cy - h/2 is the base, not the center.
    floor_y = max(0, int(blob.cy - blob.h / 2))
    if floor_y <= DETECTION_ROI[1] + 1:
        return True  # Clipped base: do not reject based on an uncertain position.
    inside = all(a * blob.cx + b * floor_y + c >= -WALL_BOUNDARY_MARGIN
                 for a, b, c in boundaries)
    if PRINT_DIAGNOSTICS and not inside:
        print("obstacle outside walls:", blob.cx, floor_y)
    return inside


def largest_valid_blob(blobs, validator=None):
    largest = None

    for blob in blobs:
        if validator is not None and not validator(blob):
            continue

        if largest is None or blob.pixels > largest.pixels:
            largest = blob

    return largest


def find_locked_obstacle(blobs, locked_obstacle):
    # Input blobs already passed the filters; match by color and nearest base.
    closest = None
    closest_distance = OBSTACLE_MATCH_DISTANCE * OBSTACLE_MATCH_DISTANCE
    floor_y = locked_obstacle.cy - locked_obstacle.h / 2

    for blob in blobs:
        if blob.code != locked_obstacle.code:
            continue
        dx = blob.cx - locked_obstacle.cx
        dy = blob.cy - blob.h / 2 - floor_y
        distance = dx * dx + dy * dy
        if distance <= closest_distance:
            closest = blob
            closest_distance = distance

    return closest


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
    camera.framerate(120)
    camera.auto_gain(False, gain_db=20.0)
    camera.snapshot()
    camera.auto_exposure(False, exposure_us=10000)
    camera.snapshot()
    camera.auto_whitebal(False, rgb_gain_db=(25, 22, 27))
    camera.brightness(0)
    camera.saturation(0)
    camera.contrast(0)

    camera.snapshot(time=2000)
    return camera


def set_ready_leds(enabled):
    led_r = pyb.LED(1)
    led_g = pyb.LED(2)
    led_b = pyb.LED(3)

    if enabled:
        led_r.on()
        led_g.on()
        led_b.on()
    else:
        led_r.off()
        led_g.off()
        led_b.off()


def main():
    set_ready_leds(False)
    camera = configure_camera()
    uart = UART(UART_ID, baudrate=UART_BAUDRATE)
    packet = bytearray(PACKET_SIZE)
    clock = time.clock()
    ready_leds_on = False
    saved_boundaries = [None, None]
    wall_last_seen = [0, 0]
    locked_obstacle = None
    obstacle_last_seen = 0

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
            roi=BLUE_DETECTION_ROI,
            pixels_threshold=MIN_LINE_PIXELS,
            area_threshold=MIN_LINE_AREA,
            merge=True,
        )
        line = largest_valid_blob(line_blobs, lambda blob: True)
        boundaries = find_wall_boundaries(
            img,
            list(obstacle_blobs) + list(wall_blobs) + list(line_blobs),
        )
        if WALL_HOLD_MS > 0:
            now = time.ticks_ms()
            for boundary in boundaries:
                # The sign of a distinguishes the image's left and right walls.
                side = 0 if boundary[0] > 0 else 1
                saved_boundaries[side] = boundary
                wall_last_seen[side] = now
            for side in range(2):
                if time.ticks_diff(now, wall_last_seen[side]) >= WALL_HOLD_MS:
                    saved_boundaries[side] = None
            boundaries = [boundary for boundary in saved_boundaries if boundary is not None]

        valid_obstacles = [blob for blob in obstacle_blobs
                           if is_obstacle_inside_walls(blob, boundaries)]
        now = time.ticks_ms()
        if locked_obstacle is None:
            # Use size only when acquiring a new target.
            obstacle = largest_valid_blob(valid_obstacles)
        else:
            obstacle = find_locked_obstacle(valid_obstacles, locked_obstacle)

        if obstacle is not None:
            locked_obstacle = obstacle
            obstacle_last_seen = now
        elif locked_obstacle is not None:
            if time.ticks_diff(now, obstacle_last_seen) >= OBSTACLE_HOLD_MS:
                obstacle = largest_valid_blob(valid_obstacles)
                locked_obstacle = obstacle
                obstacle_last_seen = now
            elif is_obstacle_inside_walls(locked_obstacle, boundaries):
                # Flicker: keep the last position without refreshing the timeout.
                obstacle = locked_obstacle

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
        img.draw_rectangle(
            BLUE_DETECTION_ROI,
            color=(255, 0, 0),
            thickness=2,
        )
        img.draw_rectangle(
            BLACK_WALL_ROI,
            color=(200, 200, 0),
            thickness=2,
        )

        # Project boundaries over the full image height after all detections.
        # a is nonzero because accepted segments have vertical extent.
        for a, b, c in boundaries:
            img.draw_line(
                (int(round(-c / a)), 0,
                 int(round(-(b * 239 + c) / a)), 239),
                color=(255, 255, 0),
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

        if not ready_leds_on:
            set_ready_leds(True)
            ready_leds_on = True
            print("Camara lista: LED blanco encendido")

        print(clock.fps())
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


try:
    main()
finally:
    set_ready_leds(False)
