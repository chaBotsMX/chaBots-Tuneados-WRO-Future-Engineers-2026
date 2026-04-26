# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Multi Color Blob Tracking Example + ROI + UART biggest blob

import sensor
import time
import math
import ustruct
import pyb
from pyb import UART


# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
thresholds = [
   (0, 50, 35, 127, -128, 127),   # Rojo
   (25, 67, -128, -31, -128, 127)  # Verde
]

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_saturation(3)
sensor.set_contrast(3)
sensor.set_auto_gain(False)       # obligatorio para color tracking
sensor.set_auto_whitebal(False)   # obligatorio para color tracking

clock = time.clock()

# UART
# Ajusta el número de UART si tu hardware usa otro
uart = UART(3, 115200)

# ROI = (x, y, w, h)
# QVGA = 320x240
# Cambia estos valores según la zona que quieras usar
ROI = (0, 0, 320, 110)

# Packet:
# [0]  0xAA   start byte
# [1]  color_id
# [2]  x low
# [3]  x high
# [4]  y low
# [5]  y high
# [6]  0x55   end byte
START_BYTE = 0xAA
END_BYTE = 0x55

def get_color_id(blob):
    # blob.code() devuelve máscara de bits:
    # 1 = thresholds[0]
    # 2 = thresholds[1]
    # 4 = thresholds[2], etc.
    if blob.code() == 1:
        return 1   # rojo
    elif blob.code() == 2:
        return 2   # verde
    else:
        return blob.code()  # por si llega a combinar colores


while True:
    clock.tick()
    img = sensor.snapshot()

    # Dibujar el ROI para referencia visual
    img.draw_rectangle(ROI, color=(255, 255, 0))

    biggest_blob = None
    biggest_pixels = 0

    blobs = img.find_blobs(
        thresholds,
        roi=ROI,
        pixels_threshold=200,
        area_threshold=200,
        merge=True
    )

    for blob in blobs:
        if blob.pixels() > biggest_pixels:
            biggest_pixels = blob.pixels()
            biggest_blob = blob

    if biggest_blob is not None:
        # Dibujos de depuración
        if biggest_blob.elongation() > 0.5:
            img.draw_edges(biggest_blob.min_corners(), color=(255, 0, 0))
            img.draw_line(biggest_blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(biggest_blob.minor_axis_line(), color=(0, 0, 255))

        img.draw_rectangle(biggest_blob.rect(), color=(255, 0, 0))
        img.draw_cross(biggest_blob.cx(), biggest_blob.cy(), color=(0, 255, 0))
        img.draw_keypoints(
            [(biggest_blob.cx(), biggest_blob.cy(), int(math.degrees(biggest_blob.rotation())))],
            size=20
        )

        color_id = get_color_id(biggest_blob)
        x = biggest_blob.cx()
        y = biggest_blob.cy()

        # Enviar paquete UART
        packet = ustruct.pack(
            "<BBHHB",
            START_BYTE,
            color_id,
            x,
            y,
            END_BYTE
        )
        uart.write(packet)

        print("Color ID:", color_id, "X:", x, "Y:", y, "Pixels:", biggest_blob.pixels())
    else:
        print("No blob")

    print("FPS:", clock.fps())
