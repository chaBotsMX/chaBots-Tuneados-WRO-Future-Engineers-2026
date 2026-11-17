"""Run Move's new distance/PID API with real motor/steering code and fake hardware."""

from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

with tempfile.TemporaryDirectory(prefix="move-cm-") as directory:
    temporary = Path(directory)
    (temporary / "Arduino.h").write_text(r"""
#pragma once
#include <cmath>
#include <cstdint>
extern uint32_t testNowUs;
extern int testAnalogPWM;
inline uint32_t micros() { return testNowUs; }
inline uint32_t millis() { return testNowUs / 1000; }
inline void delay(unsigned ms) { testNowUs += ms * 1000; }
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define constrain(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline void analogWriteFrequency(int, int) {}
inline void analogWrite(int, int value) { testAnalogPWM = value; }
struct TestSerial {
    template<class T> void print(T) {}
    template<class T> void println(T) {}
};
extern TestSerial Serial;
class elapsedMillis {
    uint32_t start = millis();
public:
    operator uint32_t() const { return millis() - start; }
    elapsedMillis& operator=(uint32_t value) { start = millis() - value; return *this; }
};
""")
    (temporary / "Encoder.h").write_text(r"""
#pragma once
extern long testTicks;
class Encoder {
public:
    Encoder(int, int) {}
    long read() { return testTicks; }
};
""")
    (temporary / "Servo.h").write_text(r"""
#pragma once
class Servo {
public:
    void attach(int) {}
    void write(float) {}
};
""")
    binary = temporary / "move_cm_test"
    subprocess.run([
        "c++", "-std=c++17", "-Wall", "-Wextra", "-Wno-unused-parameter",
        "-Wno-macro-redefined", "-fsanitize=undefined,address",
        "-I" + str(temporary), "-I" + str(ROOT / "include"),
        *["-I" + str(ROOT / "lib" / lib) for lib in ["Move", "MotorController", "AckermannController"]],
        str(ROOT / "tests/move_cm_test.cpp"),
        *[str(ROOT / "lib" / lib / (lib + ".cpp")) for lib in ["Move", "MotorController", "AckermannController"]],
        "-o", str(binary),
    ], check=True)
    subprocess.run([str(binary)], check=True)
