// Run with: python3 tests/check_obstacle_evasion_integration.py --suite depth
#include "TrajectoryController.h"
#include "fixtures/tangent_evasion_legacy.h"
#include <assert.h>
#include <initializer_list>
#include <stdio.h>

uint32_t testNow = 1000;

static float run(TrajectoryController& tc, float y, float direction = -1.0f,
                 float imu = 0.0f, float angle = 63.43495f, float distance = 111.8034f) {
    return tc.tangentEvasion(imu, direction, angle, TAN_EVASION_SECURITY_RADIUS_PX, distance, y);
}

static void matchesLegacy(bool allDepths) {
    TrajectoryController current;
    LegacyTangentEvasion legacy;
    const uint32_t steps[] = {0, 1, 2, 7, 16, 67, 120, 501};
    for (unsigned i = 0; i < 2000; ++i) {
        testNow += steps[i % 8];
        const float y = allDepths ? 30.0f + i % 95 : 100.0f + i % 25;
        const float lateral = float(int(i % 281) - 140);
        const float distance = hypotf(lateral, y);
        const float angle = degrees(atan2f(lateral, y));
        const float direction = i % 3 == 0 ? 1.0f : -1.0f;
        const float imu = float(int(i % 61) - 30);
        if (i % 31 == 0) {
            current.resetTangentEvasion(imu);
            legacy.resetTangentEvasion(imu);
        }
        const float actual = run(current, y, direction, imu, angle, distance);
        const float expected = legacy.tangentEvasion(imu, direction, angle,
            TAN_EVASION_SECURITY_RADIUS_PX, distance);
        assert(actual == expected);
        assert(current.lastDepthGain == 1.0f);
    }
}

static void depthCurveAndResets() {
    const float startY = TAN_EVASION_DEPTH_START_Y_PX;
    const float fullY = TAN_EVASION_DEPTH_FULL_Y_PX;
    const float maxGain = TAN_EVASION_DEPTH_GAIN_MAX;
    for (float y : {startY + 20, startY, (startY + fullY) / 2, fullY, fullY - 5}) {
        TrajectoryController tc;
        run(tc, y);
        const float expected = y >= startY ? 1.0f : y <= fullY ? maxGain : (1 + maxGain) / 2;
        assert(fabsf(tc.lastDepthGain - expected) < 0.00001f);
    }
    TrajectoryController tc;
    run(tc, fullY); assert(tc.lastDepthGain == maxGain);
    run(tc, startY, 1.0f); assert(tc.lastDepthGain == 1); // New color, no inherited boost.
    tc.resetTangentEvasion(); run(tc, fullY); assert(tc.lastDepthGain == maxGain);
    tc.resetDepthGain(); run(tc, startY); assert(tc.lastDepthGain == 1);
    for (float y : {-1.0f, float(VISION_HEIGHT), float(NAN), float(INFINITY)}) {
        run(tc, y); assert(tc.lastDepthGain == 1);
        run(tc, fullY); assert(tc.lastDepthGain == maxGain);
    }
    tc.tangentEvasion(3, -1, 0, 110, 0, fullY);
    assert(tc.lastDepthGain == 1);
}

static void smoothingUsesElapsedTime() {
    const uint32_t start = testNow;
    TrajectoryController single, repeated;
    run(single, 100); run(repeated, 100);
    testNow = start + 60; run(single, 40);
    for (unsigned i = 1; i <= 6; ++i) {
        testNow = start + i * 10; run(repeated, 40);
    }
    assert(fabsf(single.lastDepthGain - repeated.lastDepthGain) < 0.00001f);
    assert(single.lastDepthGain > 1 && single.lastDepthGain < TAN_EVASION_DEPTH_GAIN_MAX);
    const float prior = single.lastDepthGain;
    run(single, 40); assert(single.lastDepthGain == prior); // Same millisecond, no filter advance.
    testNow += 60; run(single, 100);
    assert(single.lastDepthGain < prior && single.lastDepthGain >= 1);
}

static void boostDoesNotEnterDerivativeOrHeading() {
    TrajectoryController current;
    LegacyTangentEvasion legacy;
    const float direction = -1, imu = 5, angle = 63.43495f, distance = 111.8034f;
    const float evasion = TAN_EVASION_EVASION_GAIN *
        (angle - degrees(asinf(TAN_EVASION_SECURITY_RADIUS_PX / distance)));
    for (float y : {100, 40, 30, 65, 40, 100}) {
        testNow += 17;
        const float actual = run(current, y, direction, imu, angle, distance);
        const float baseline = legacy.tangentEvasion(imu, direction, angle,
            TAN_EVASION_SECURITY_RADIUS_PX, distance);
        const float expectedExtraP = TAN_EVASION_KP * (current.lastDepthGain - 1) * evasion;
        assert(fabsf(actual - baseline - expectedExtraP) < 0.0001f);
    }
    TrajectoryController a, b;
    const float noHeading = run(a, 40, -1, 0);
    const float withHeading = run(b, 40, -1, 10);
    assert(fabsf(withHeading - noHeading - TAN_EVASION_KP * TAN_EVASION_ORIENTATION_GAIN * 10) < 0.0001f);
}

static void holdsMaximumLongerAndMirrors() {
    TrajectoryController tc;
    LegacyTangentEvasion legacy;
    const float upgraded = run(tc, 50);
    const float baseline = legacy.tangentEvasion(0, -1, 63.43495f,
        TAN_EVASION_SECURITY_RADIUS_PX, 111.8034f);
    assert(baseline > -25.94f && upgraded < -25.94f);
    printf("Close lateral block: requested %.2f -> %.2f degrees; applied %.2f -> -25.94\n",
        baseline, upgraded, baseline);

    TrajectoryController red, green;
    for (unsigned i = 0; i < 200; ++i) {
        testNow += 7 + i % 30;
        const float y = 30.0f + i % 95;
        const float x = float(i % 131);
        const float angle = degrees(atan2f(x, y));
        const float distance = hypotf(x, y);
        const float imu = float(int(i % 21) - 10);
        const float r = run(red, y, -1, imu, angle, distance);
        const float g = run(green, y, 1, -imu, -angle, distance);
        assert(fabsf(r + g) < 0.001f);
        assert(red.lastDepthGain == green.lastDepthGain);
    }
}

static void returnCorrectionIsNotAmplified() {
    for (float direction : {-1.0f, 1.0f}) {
        TrajectoryController current;
        LegacyTangentEvasion legacy;
        // Lateral clearance exceeds the radius while the obstacle remains close in Y.
        // The IMU can keep this block in the evasion branch; preserve its return correction.
        for (float lateral : {100.0f, 110.0f, 120.0f, 140.0f}) {
            testNow += 17;
            const float y = 40;
            const float angle = -direction * degrees(atan2f(lateral, y));
            const float distance = hypotf(lateral, y);
            const float actual = run(current, y, direction, 20 * direction, angle, distance);
            const float baseline = legacy.tangentEvasion(20 * direction, direction, angle,
                TAN_EVASION_SECURITY_RADIUS_PX, distance);
            if (lateral > TAN_EVASION_SECURITY_RADIUS_PX) assert(actual == baseline);
            else assert(direction * (actual - baseline) >= 0);
        }
    }
}

int main() {
#ifdef EXPECT_BASELINE_ONLY
    matchesLegacy(true);
    puts("Depth boost disabled/invalid config: 2000 outputs exactly match the old controller");
#else
    matchesLegacy(false);
    depthCurveAndResets();
    smoothingUsesElapsedTime();
    boostDoesNotEnterDerivativeOrHeading();
    holdsMaximumLongerAndMirrors();
    returnCorrectionIsNotAmplified();
    puts("Depth gain: 2000 far outputs unchanged; curve, timing, resets, P-only boost and mirror checks passed");
#endif
}
