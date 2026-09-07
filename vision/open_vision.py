import csi
import pyb
import time
from machine import UART

LINE_THRESHOLDS = [
    (11, 20, -128, 127, -128, 127),  # ID 1: blue
    (23, 82, 12, 127, -128, 39)  # ID 2: orange
]

UART_ID = 3
UART_BAUDRATE = 115200

START_BYTE_HIGH = 0xAA
START_BYTE_LOW = 0x55
PACKET_SIZE = 3

DETECTION_ROI = (40, 30, 240, 50)
MIN_LINE_PIXELS = 120
MIN_LINE_AREA = 40

PRINT_DIAGNOSTICS = False


def largest_blob(blobs):
    largest = None

    for blob in blobs:
        if largest is None or blob.pixels > largest.pixels:
            largest = blob

    return largest


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
    camera.auto_whitebal(False, rgb_gain_db=(26, 22, 27))
    camera.brightness(-1)
    camera.saturation(0)
    camera.contrast(-1)

    if camera.cid() in (csi.OV7725, csi.OV5640):
        camera.ioctl(csi.IOCTL_SET_NIGHT_MODE, False)

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
    packet = bytearray((START_BYTE_HIGH, START_BYTE_LOW, 0))
    clock = time.clock()
    ready_leds_on = False

    while True:
        clock.tick()
        img = camera.snapshot()

        line_blobs = img.find_blobs(
            LINE_THRESHOLDS,
            roi=DETECTION_ROI,
            pixels_threshold=MIN_LINE_PIXELS,
            area_threshold=MIN_LINE_AREA,
            merge=False,
        )
        detected_blob = largest_blob(line_blobs)

        if detected_blob is not None:
            detected_id = detected_blob.code
            packet[2] = detected_id
            uart.write(packet)

            if detected_id == 1:
                detected_color = (0, 0, 255)
            elif detected_id == 2:
                detected_color = (255, 128, 0)
            else:
                detected_color = (255, 255, 255)

            img.draw_rectangle(
                detected_blob.rect,
                color=detected_color,
                thickness=2,
            )


        else:
            packet[2] = 3

            uart.write(packet)



        img.draw_rectangle(
            DETECTION_ROI,
            color=(255, 0, 0),
            thickness=2,
        )

        if not ready_leds_on:
            set_ready_leds(True)
            ready_leds_on = True
            print("Camara lista: LED blanco encendido")

        if PRINT_DIAGNOSTICS:
            print("fps:", clock.fps())


try:
    main()
finally:
    set_ready_leds(False)
