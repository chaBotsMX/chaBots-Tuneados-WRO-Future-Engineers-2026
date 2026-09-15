"""Run the real evasion branch and tangential controller with simulated sensor inputs."""

from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "lib/Robot/Robot.cpp").read_text()
header = (ROOT / "lib/Robot/Robot.h").read_text()
start = source.index("void Robot::executeEvadeUntilEdge()")
end = source.index("void Robot::setEvadeUntilEdge()", start)

def method(name):
    match = re.search(r"void Robot::" + name + r"\([^)]*\)\s*\{", source)
    index, depth = match.end(), 1
    while depth:
        depth += (source[index] == "{") - (source[index] == "}")
        index += 1
    return source[match.start():index]

with tempfile.TemporaryDirectory(prefix="obstacle-evasion-") as directory:
    temporary = Path(directory)
    (temporary / "robot_evade_under_test.inc").write_text(source[start:end])
    (temporary / "robot_startup_under_test.inc").write_text("\n".join(method(name) for name in [
        "executeTaskObstacles", "decideDirObstacles", "setEvadeUntilEdge",
        "setApproachBlueLine", "setReverseAfterBlueLine", "setImuSetPoint",
    ]))
    (temporary / "robot_startup_constants.inc").write_text("\n".join(
        re.findall(r"static constexpr [^;]+;", header)
        + [re.search(r"enum class TASK\s*\{.*?\};", header, re.S).group()]
        + [re.search(r"uint16_t CAMERA_DATA_TIMEOUT_MS[^;]+;", header).group()]
    ))
    (temporary / "Arduino.h").write_text(r"""
#pragma once
#include <math.h>
#include <stdint.h>
extern uint32_t testNow;
inline uint32_t millis() { return testNow; }
inline float degrees(float radians) { return radians * (180.0f / 3.14159265358979323846f); }
template<class T> T constrain(T value, T low, T high) {
    return value < low ? low : value > high ? high : value;
}
class elapsedMillis {
    uint32_t start;
public:
    elapsedMillis(uint32_t value = 0) : start(testNow - value) {}
    operator uint32_t() const { return testNow - start; }
    elapsedMillis& operator=(uint32_t value) { start = testNow - value; return *this; }
};
""")
    for test in ["obstacle_evasion_integration", "parking_direction_integration"]:
        binary = temporary / test
        subprocess.run([
            "c++", "-std=c++11", "-Wno-macro-redefined",
            "-I" + str(temporary), "-I" + str(ROOT / "include"),
            "-I" + str(ROOT / "lib/Robot"), "-I" + str(ROOT / "lib/TrajectoryController"),
            str(ROOT / "tests" / (test + ".cpp")),
            str(ROOT / "lib/TrajectoryController/TrajectoryController.cpp"),
            "-o", str(binary),
        ], check=True)
        subprocess.run([str(binary)], check=True)
