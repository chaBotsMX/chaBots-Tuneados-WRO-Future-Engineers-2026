import csi
import math
import pyb
import time
from machine import UART


OBSTACLE_THRESHOLDS = [
    (8, 29, 14, 127, -128, 127),
    (0, 49, -128, -12, 2, 127)  # Verde
]

PARKING_WALL_THRESHOLDS = [
    (0, 34, 13, 127, -128, 4)    # Magenta
]

BLUE_LINE_THRESHOLD = [
    (0, 79, -128, 20, -128, -7),
]

BLACK_WALL_THRESHOLD = [(0, 25, -12, 12, -12, 12)]  # Calibrar negro
BLACK_WALL_ROI = (0, 0, 320, 50)  # Pared cercana; proyectar hacia los pilares
WALL_FLOOR_REFERENCE = (160, 4)  # Suelo cercano, camara invertida
MIN_WALL_LINE_LENGTH = 25
MIN_WALL_LINE_HEIGHT = 18
WALL_SAMPLE_DISTANCE = 4
WALL_BOUNDARY_MARGIN = 6  # Pixeles; tolerancia, no distancia al robot

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

PRINT_DIAGNOSTICS = False


def is_vertical_obstacle(blob):
    is_vertical = blob.h > blob.w * MIN_OBSTACLE_ASPECT_RATIO
    if PRINT_DIAGNOSTICS:
        print("vertical obstacle:", is_vertical)
    return is_vertical


def is_horizontal_parking_wall(blob):
    return blob.w > blob.h * MIN_WALL_ASPECT_RATIO


def find_wall_boundaries(img, color_blobs):
    # Blanco en la mascara = pared negra. No modificar la imagen original.
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
            if blob.x <= x < blob.x + blob.w and blob.y <= y < blob.y + blob.h:
                return None
        return mask.get_pixel((x, y)) > 127

    if is_wall(reference_x, reference_y) is not False:
        return []

    for line in mask.find_line_segments(merge_distance=5, max_theta_difference=10):
        x1, y1, x2, y2 = line[0], line[1], line[2], line[3]
        dx, dy = x2 - x1, y2 - y1
        length = math.sqrt(dx * dx + dy * dy)
        if length < MIN_WALL_LINE_LENGTH or abs(dy) < MIN_WALL_LINE_HEIGHT:
            continue

        # a*x + b*y + c es distancia firmada; positivo hacia nuestro suelo.
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
            # El suelo debe continuar hacia el robot: evita usar el borde superior.
            if all(is_wall(x + (reference_x - x) * step / 5,
                           y + (reference_y - y) * step / 5) is False
                   for step in range(1, 5)):
                valid_samples += 1

        if valid_samples < 4:
            continue

        near_x = x1 + dx * (reference_y - y1) / dy
        side = 0 if near_x < reference_x else 1
        # Preferir el borde largo mas cercano, no la cara despues de la esquina.
        score = length / (1 + min(y1, y2))
        if score > scores[side]:
            scores[side] = score
            boundaries[side] = (a, b, c)

    return [boundary for boundary in boundaries if boundary is not None]


def is_obstacle_inside_walls(blob, boundaries):
    if not is_vertical_obstacle(blob):
        return False
    # Con 180 grados, cy - h/2 es la base. No usar el centro del pilar.
    floor_y = max(0, int(blob.cy - blob.h / 2))
    if floor_y <= DETECTION_ROI[1] + 1:
        return True  # Base recortada: no rechazar por una posicion incierta.
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
    camera.auto_gain(False, gain_db=15.0)
    camera.snapshot()
    camera.auto_exposure(False, exposure_us=9000)
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
        obstacle = largest_valid_blob(
            obstacle_blobs,
            lambda blob: is_obstacle_inside_walls(blob, boundaries),
        )
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

        # Proyectar los limites a toda la altura, despues de las detecciones.
        # a no es cero: los segmentos aceptados tienen extension vertical.
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
