#pragma once

#include <math.h>
#include <stdint.h>
#include "ControlValues.h"

namespace ObstacleEvasion {

constexpr uint8_t RED = 1;
constexpr uint8_t GREEN = 2;
constexpr float MAX_PASS_HEADING_ERROR_DEG = 15.0f;
constexpr float PARKING_ACTIVATION_ANGLE_DEG = 15.0f;
constexpr float PARKING_STEERING_ANGLE_DEG = 15.0f;

inline bool validPosition(uint16_t x, uint16_t y) {
    return x < VISION_WIDTH && y < VISION_HEIGHT;
}

inline bool validColor(uint8_t color) {
    return color == RED || color == GREEN;
}

// The camera is rotated 180 degrees: image-right is robot-left.
// Keep the same image-space geometry and clearance radius as tangentEvasion.
// Being on the correct side alone does not mean the block has been cleared.
inline bool canIgnoreObstacle(uint16_t x, uint16_t y, uint8_t color,
                              float headingErrorDeg, float clearancePx) {
    if (!validPosition(x, y) || !validColor(color) ||
        !isfinite(headingErrorDeg) || !isfinite(clearancePx) || clearancePx <= 0.0f ||
        fabsf(headingErrorDeg) >= MAX_PASS_HEADING_ERROR_DEG) {
        return false;
    }

    const float lateralPx = float(x) - VISION_WIDTH / 2.0f;
    const float headingRad = headingErrorDeg * (3.14159265358979323846f / 180.0f);
    // Check clearance both now and along the heading the IMU will restore.
    const float lateralAtTargetPx = lateralPx * cosf(headingRad) - float(y) * sinf(headingRad);
    const float obstacleSide = color == RED ? 1.0f : -1.0f;
    return obstacleSide * lateralPx >= clearancePx &&
           obstacleSide * lateralAtTargetPx >= clearancePx;
}

// Parking is next to the outer wall: turn toward the track interior.
// Before decideDir, use the visible side instead of assuming the default course direction.
inline float parkingSteering(uint16_t x, uint16_t y, bool clockwise, bool directionKnown) {
    if (!validPosition(x, y)) return 0.0f;

    const float lateralPx = float(x) - VISION_WIDTH / 2.0f;
    const float wallAngle = atan2f(lateralPx, float(y)) * (180.0f / 3.14159265358979323846f);
    float turnSign = clockwise ? -1.0f : 1.0f;
    if (!directionKnown && lateralPx != 0.0f) {
        turnSign = lateralPx > 0.0f ? -1.0f : 1.0f;
    }
    return turnSign * wallAngle > -PARKING_ACTIVATION_ANGLE_DEG
        ? turnSign * PARKING_STEERING_ANGLE_DEG : 0.0f;
}

} // namespace ObstacleEvasion
