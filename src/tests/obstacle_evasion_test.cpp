// Native check: c++ -std=c++11 -Ilib/Robot -Iinclude tests/obstacle_evasion_test.cpp -o /tmp/obstacle-evasion-test && /tmp/obstacle-evasion-test
#include "ObstacleEvasion.h"

#include <assert.h>
#include <initializer_list>
#include <stdio.h>

using namespace ObstacleEvasion;

static bool ignore(uint16_t x, uint16_t y, uint8_t color, float heading = 0.0f) {
    return canIgnoreObstacle(x, y, color, heading, TAN_EVASION_SECURITY_RADIUS_PX);
}

static void checksPassingClearance() {
    // Rotated camera: red stays on robot-left (image-right), green on robot-right.
    assert(ignore(290, 80, RED));
    assert(ignore(30, 80, GREEN));
    assert(!ignore(290, 80, GREEN));
    assert(!ignore(30, 80, RED));

    // A block can exceed the old 10-degree threshold while still being too close.
    assert(!ignore(210, 80, RED));
    assert(!ignore(110, 80, GREEN));
    assert(!ignore(160, 80, RED));
    assert(!ignore(160, 80, GREEN));
    assert(ignore(160 + TAN_EVASION_SECURITY_RADIUS_PX, 80, RED));
    assert(ignore(160 - TAN_EVASION_SECURITY_RADIUS_PX, 80, GREEN));
    assert(!ignore(159 + TAN_EVASION_SECURITY_RADIUS_PX, 80, RED));
    assert(!ignore(161 - TAN_EVASION_SECURITY_RADIUS_PX, 80, GREEN));

    // Returning toward a block must retain evasion, even if today's lateral gap is clear.
    assert(!ignore(280, 100, RED, 10.0f));
    assert(ignore(280, 100, RED, -10.0f));
    assert(!ignore(40, 100, GREEN, -10.0f));
    assert(ignore(40, 100, GREEN, 10.0f));
    for (float heading : {-90.0f, -15.0f, 15.0f, 90.0f, float(NAN), float(INFINITY)}) {
        assert(!ignore(290, 80, RED, heading));
        assert(!ignore(30, 80, GREEN, heading));
    }

    assert(!ignore(290, 80, 0));
    assert(!ignore(290, 80, 3));
    assert(!ignore(320, 80, RED));
    assert(!ignore(290, 240, RED));
    assert(!ignore(250, 250, RED));
    assert(!canIgnoreObstacle(290, 80, RED, 0, 0));
}

static void checksParkingDirections() {
    // CW: outer wall on robot-left. CCW: outer wall on robot-right.
    assert(parkingSteering(180, 100, true, true) == -15.0f);
    assert(parkingSteering(140, 100, false, true) == 15.0f);
    assert(parkingSteering(260, 100, true, true) == 0.0f);
    assert(parkingSteering(60, 100, false, true) == 0.0f);
    assert(parkingSteering(160, 100, true, true) == -15.0f);
    assert(parkingSteering(160, 100, false, true) == 15.0f);

    // Before the first direction decision, a visible side overrides either default.
    for (bool clockwise : {false, true}) {
        assert(parkingSteering(180, 100, clockwise, false) == -15.0f);
        assert(parkingSteering(140, 100, clockwise, false) == 15.0f);
        assert(parkingSteering(250, 250, clockwise, true) == 0.0f);
        assert(parkingSteering(320, 80, clockwise, true) == 0.0f);
    }
}

static void checksMirroredCases() {
    unsigned comparisons = 0;
    for (uint16_t x = 1; x < VISION_WIDTH; ++x) {
        for (uint16_t y : {0, 30, 80, 115, 200, 239}) {
            for (float heading : {-90.0f, -15.0f, -10.0f, 0.0f, 10.0f, 15.0f, 90.0f}) {
                assert(ignore(x, y, RED, heading) ==
                       ignore(VISION_WIDTH - x, y, GREEN, -heading));
                ++comparisons;
            }
            for (bool known : {false, true}) {
                assert(parkingSteering(x, y, true, known) ==
                       -parkingSteering(VISION_WIDTH - x, y, false, known));
                ++comparisons;
            }
        }
    }
    printf("ObstacleEvasion: passing clearance, parking and %u mirrored comparisons passed\n", comparisons);
}

int main() {
    checksPassingClearance();
    checksParkingDirections();
    checksMirroredCases();
}
