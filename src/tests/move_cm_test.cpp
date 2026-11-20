#include "Move.h"
#include "AckermannController.h"
#include <cassert>
#include <cstdio>
#include <limits>

uint32_t testNowUs = 0;
long testTicks = 0;
int testAnalogPWM = 0;
TestSerial Serial;

static void step(Move& move, long ticks = 0, uint32_t us = 20000) {
    testTicks += ticks;
    testNowUs += us;
    move.executeMoving();
}

int main() {
    AckermannController steering;
    Move move(steering);
    move.begin();
    move.driveAtPWM(17);
    assert(move.executeMoving()); // Idle callback must not overwrite another state.
    assert(move.controller.lastPwm == 17);

    testTicks = -1000;
    move.controller.resetTicks();
    testTicks -= 50;
    const long oldRelativeTicks = move.controller.getTicks();
    assert(move.moveByCm(0.5f, 5.0f));
    assert(move.controller.getTicks() == oldRelativeTicks);
    assert(steering.lastSteeringAngleDeg == 0.0f);
    assert(!move.executeMoving()); // No division by zero at the same micros().
    step(move);
    const int firstPWM = move.controller.lastPwm;
    assert(firstPWM < 0);
    for (int i = 0; i < 20; ++i) step(move); // Stationary encoder / backlash.
    assert(move.controller.lastPwm < firstPWM); // Integral overcomes low PWM.
    step(move, 40); // Wrong direction must not complete the task.
    assert(move.isMoving() && move.getMotionDistanceCm() < 0.0f);
    step(move, -60);
    assert(move.isMoving()); // Fractional distance is not truncated to zero.
    move.controller.resetTicks(); // External relative resets do not affect progress.
    step(move, -20);
    assert(!move.isMoving() && move.executeMoving());
    assert(move.controller.lastPwm == 0 && testAnalogPWM == MAX_PWM);
    move.driveAtPWM(31);
    assert(move.executeMoving() && move.controller.lastPwm == 31);

    assert(move.turnByCm(-1.0f, 8.0f, 25.0f));
    assert(steering.lastSteeringAngleDeg == 25.0f);
    step(move);
    assert(move.controller.lastPwm > 0);
    step(move, 10);
    assert(move.getMotionSpeedCmS() < 0.0f && move.isMoving());
    step(move, 70);
    assert(!move.isMoving() && move.getMotionDistanceCm() <= -1.0f);
    assert(move.moveByCm(10.0f, 2.0f));
    assert(steering.lastSteeringAngleDeg == 0.0f);
    step(move, -100);
    assert(move.moveByCm(-0.1f, 2.0f)); // Replacement uses a new baseline.
    step(move, 8);
    assert(!move.isMoving());

    MovePIDConfig config;
    config.kp = 1.0f;
    config.ki = 30.0f;
    config.kd = 0.0f;
    config.maxPWM = 40;
    move.configureMotionPID(config);
    assert(move.moveByCm(1000.0f, 5.0f));
    for (int i = 0; i < 500; ++i) {
        step(move);
        assert(move.controller.lastPwm >= -40 && move.controller.lastPwm <= 0);
    }
    step(move, -1000); // Overspeed after prolonged stall: no saturated integral tail.
    assert(move.controller.lastPwm == 0);
    move.cancelMoving();
    assert(move.executeMoving() && !move.isMoving());
    assert(move.moveByCm(1000.0f, 5.0f));
    step(move);
    assert(move.controller.lastPwm == -8); // Integral was reset between tasks.

    config.kp = 0.0f;
    config.ki = 0.0f;
    config.kd = 0.1f;
    move.configureMotionPID(config);
    assert(move.moveByCm(1000.0f, 5.0f));
    step(move, -100);
    assert(move.controller.lastPwm == 0); // Rising speed reduces output via D.
    step(move);
    assert(move.controller.lastPwm < 0); // Falling speed increases output via D.

    config.pwmForwardSign = 1;
    config.encoderForwardSign = 1;
    config.kp = 2.0f;
    config.ki = 0.0f;
    config.kd = 0.0f;
    move.configureMotionPID(config);
    testNowUs = UINT32_MAX - 10000U;
    testTicks = INT32_MAX - 50;
    assert(move.moveByCm(10.0f, 5.0f));
    step(move); // micros() rollover.
    assert(move.controller.lastPwm == 10);
    testTicks = static_cast<int32_t>(static_cast<uint32_t>(testTicks) + 100U);
    step(move);
    assert(move.getMotionDistanceCm() > 0.1f); // Encoder rollover.
    testTicks += 700;
    assert(move.executeMoving());

    assert(move.moveByCm(0.0f, 5.0f) && move.executeMoving());
    assert(!move.moveByCm(10.0f, 0.0f) && move.executeMoving());
    assert(!move.moveByCm(10.0f, -5.0f));
    assert(!move.moveByCm(std::numeric_limits<float>::quiet_NaN(), 5.0f));
    assert(!move.turnByCm(10.0f, 5.0f, std::numeric_limits<float>::infinity()));
    config.sampleTimeMs = 0;
    move.configureMotionPID(config);
    assert(!move.moveByCm(10.0f, 5.0f));

    Move legacy;
    assert(!legacy.turnByCm(10.0f, 5.0f, 20.0f));
    legacy.setSteeringController(steering);
    assert(legacy.turnByCm(10.0f, 5.0f, 20.0f));
    legacy.cancelMoving();
    Move configured{MovePIDConfig()};
    assert(configured.moveByCm(1.0f, 5.0f));
    configured.cancelMoving();
    std::puts("Move CM: distance, steering, PID, cancellation and rollover passed.");
}
