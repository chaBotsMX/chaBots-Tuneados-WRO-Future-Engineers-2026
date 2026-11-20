#pragma once

#include <stdint.h>

// Observes the initial straight approach; never commands motion.
class OpenDirectionMonitor {
public:
    enum class Side { LEFT, RIGHT };
    enum class Gap { UNCONFIRMED, LEFT, RIGHT };

    static constexpr uint16_t NEAR_WALL_MM = 200;
    static constexpr uint16_t LOST_WALL_MM = 300;
    static constexpr uint16_t WALL_TOLERANCE_MM = 50;
    static constexpr uint8_t CONFIRM_SAMPLES = 3;
    static constexpr uint32_t INITIAL_WINDOW_MS = 500;
    static constexpr uint32_t MAX_SAMPLE_AGE_MS = 200;

    void begin(uint32_t nowMs) {
        walls[0] = Wall{};
        walls[1] = Wall{};
        startedAtMs = nowMs;
        started = true;
    }

    // A maneuver breaks the initial wall reference. Only a new start can rearm it.
    void invalidate() {
        started = false;
    }

    // Call exactly once per fresh ToF frame, including fresh invalid readings.
    // A sensor that stops producing frames must not count as an opening.
    void observe(Side side, uint16_t distanceMm, bool valid, uint32_t nowMs) {
        if (!started) return;

        Wall& wall = walls[side == Side::LEFT ? 0 : 1];
        if (wall.hasSample && uint32_t(nowMs - wall.sampledAtMs) > MAX_SAMPLE_AGE_MS) {
            wall.nearSamples = 0;
            wall.nearSum = 0;
            wall.presentSamples = 0;
            wall.lostSamples = 0;
        }
        wall.hasSample = true;
        wall.sampledAtMs = nowMs;

        if (!wall.hasReference) {
            // Do not learn a different wall after reaching the corner.
            if (uint32_t(nowMs - startedAtMs) > INITIAL_WINDOW_MS) return;
            if (!valid || distanceMm >= NEAR_WALL_MM) {
                wall.nearSamples = 0;
                wall.nearSum = 0;
                return;
            }

            const uint16_t low = wall.nearSamples == 0 || distanceMm < wall.nearMin
                ? distanceMm : wall.nearMin;
            const uint16_t high = wall.nearSamples == 0 || distanceMm > wall.nearMax
                ? distanceMm : wall.nearMax;
            if (high - low > WALL_TOLERANCE_MM) {
                wall.nearSamples = 0;
                wall.nearSum = 0;
            }
            wall.nearMin = wall.nearSamples == 0 ? distanceMm : low;
            wall.nearMax = wall.nearSamples == 0 ? distanceMm : high;
            wall.nearSum += distanceMm;
            if (++wall.nearSamples == CONFIRM_SAMPLES) {
                wall.referenceMm = wall.nearSum / CONFIRM_SAMPLES;
                wall.hasReference = true;
                wall.presentSamples = CONFIRM_SAMPLES;
            }
            return;
        }

        const int difference = int(distanceMm) - int(wall.referenceMm);
        if (valid && difference >= -int(WALL_TOLERANCE_MM) &&
                     difference <= int(WALL_TOLERANCE_MM)) {
            if (wall.presentSamples < CONFIRM_SAMPLES) ++wall.presentSamples;
            wall.lostSamples = 0;
        } else if (!valid || distanceMm >= LOST_WALL_MM) {
            if (wall.lostSamples < CONFIRM_SAMPLES) ++wall.lostSamples;
            wall.presentSamples = 0;
        } else {
            // An intermediate distance is uncertain, not a confirmed gap.
            wall.presentSamples = 0;
            wall.lostSamples = 0;
        }
    }

    Gap gap(uint32_t nowMs) const {
        if (!started) return Gap::UNCONFIRMED;
        for (const Wall& wall : walls) {
            if (!wall.hasReference || !wall.hasSample ||
                uint32_t(nowMs - wall.sampledAtMs) > MAX_SAMPLE_AGE_MS) {
                return Gap::UNCONFIRMED;
            }
        }
        if (walls[0].lostSamples >= CONFIRM_SAMPLES &&
            walls[1].presentSamples >= CONFIRM_SAMPLES) return Gap::LEFT;
        if (walls[1].lostSamples >= CONFIRM_SAMPLES &&
            walls[0].presentSamples >= CONFIRM_SAMPLES) return Gap::RIGHT;
        return Gap::UNCONFIRMED;
    }

private:
    struct Wall {
        uint32_t sampledAtMs = 0;
        uint16_t referenceMm = 0;
        uint16_t nearSum = 0;
        uint16_t nearMin = 0;
        uint16_t nearMax = 0;
        uint8_t nearSamples = 0;
        uint8_t presentSamples = 0;
        uint8_t lostSamples = 0;
        bool hasReference = false;
        bool hasSample = false;
    };

    Wall walls[2];
    uint32_t startedAtMs = 0;
    bool started = false;
};
