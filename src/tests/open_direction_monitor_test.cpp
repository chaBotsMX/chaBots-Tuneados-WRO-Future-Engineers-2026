// Native check: c++ -std=c++11 -Ilib/Robot tests/open_direction_monitor_test.cpp -o /tmp/open-direction-test && /tmp/open-direction-test
#include "OpenDirectionMonitor.h"

#include <assert.h>
#include <initializer_list>
#include <stdio.h>

using Monitor = OpenDirectionMonitor;
using Side = Monitor::Side;
using Gap = Monitor::Gap;

static void pair(Monitor& monitor, uint32_t now, uint16_t left, uint16_t right) {
    monitor.observe(Side::LEFT, left, left < 3000, now);
    monitor.observe(Side::RIGHT, right, right < 3000, now);
}

static void startWithWalls(Monitor& monitor, uint32_t start = 0) {
    monitor.begin(start);
    pair(monitor, start + 10, 145, 155);
    pair(monitor, start + 77, 150, 150);
    pair(monitor, start + 144, 155, 145);
}

static void confirmsBothDirections() {
    Monitor left;
    startWithWalls(left);
    pair(left, 300, 3001, 150);
    pair(left, 367, 3001, 150);
    // Repeated control loops are not new sensor measurements.
    for (uint32_t now = 367; now < 434; ++now) {
        assert(left.gap(now) == Gap::UNCONFIRMED);
    }
    pair(left, 434, 3001, 150);
    assert(left.gap(434) == Gap::LEFT);

    Monitor right;
    startWithWalls(right);
    // Distant readings need not be accurate or agree with one another.
    pair(right, 300, 150, 400);
    pair(right, 367, 150, 900);
    pair(right, 434, 150, 3001);
    assert(right.gap(434) == Gap::RIGHT);
}

static void rejectsNoiseAndAmbiguity() {
    Monitor monitor;
    startWithWalls(monitor);
    pair(monitor, 300, 3001, 150);
    pair(monitor, 367, 150, 150);
    pair(monitor, 434, 3001, 150);
    pair(monitor, 501, 3001, 150);
    assert(monitor.gap(501) == Gap::UNCONFIRMED);
    pair(monitor, 568, 250, 150); // Uncertain range breaks the loss streak.
    pair(monitor, 635, 3001, 150);
    assert(monitor.gap(635) == Gap::UNCONFIRMED);

    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 3001);
    assert(monitor.gap(434) == Gap::UNCONFIRMED);

    // The opposite side must still be at its original nearby wall.
    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 240);
    assert(monitor.gap(434) == Gap::UNCONFIRMED);
}

static void rejectsMissingOrStaleReferences() {
    Monitor monitor;
    assert(monitor.gap(0) == Gap::UNCONFIRMED);
    monitor.begin(0);
    for (uint32_t now : {10u, 77u, 144u}) pair(monitor, now, 800, 150);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(434) == Gap::UNCONFIRMED);

    // A nearby wall first seen late must not become an initial reference.
    monitor.begin(0);
    for (uint32_t now : {600u, 667u, 734u}) pair(monitor, now, 150, 150);
    for (uint32_t now : {800u, 867u, 934u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(934) == Gap::UNCONFIRMED);

    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(635) == Gap::UNCONFIRMED);

    // Fresh invalid frames on the gap side cannot replace a stopped opposite sensor.
    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) {
        monitor.observe(Side::LEFT, 3001, false, now);
    }
    assert(monitor.gap(434) == Gap::UNCONFIRMED);

    // A break in acquisition also breaks a partial disappearance streak.
    startWithWalls(monitor);
    pair(monitor, 300, 3001, 150);
    pair(monitor, 367, 3001, 150);
    pair(monitor, 600, 3001, 150);
    assert(monitor.gap(600) == Gap::UNCONFIRMED);
}

static void keepsReferenceAndResets() {
    Monitor monitor;
    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 800, 150);
    assert(monitor.gap(434) == Gap::LEFT);
    for (uint32_t now : {501u, 568u, 635u}) pair(monitor, now, 150, 3001);
    assert(monitor.gap(635) == Gap::RIGHT);

    monitor.begin(1000);
    for (uint32_t now : {1010u, 1077u, 1144u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(1144) == Gap::UNCONFIRMED);
}

static void handlesThresholdsAndClockWrap() {
    Monitor monitor;
    monitor.begin(0);
    for (uint32_t now : {10u, 77u, 144u}) pair(monitor, now, 200, 150);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(434) == Gap::UNCONFIRMED);

    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 299, 150);
    assert(monitor.gap(434) == Gap::UNCONFIRMED);
    for (uint32_t now : {501u, 568u, 635u}) pair(monitor, now, 300, 150);
    assert(monitor.gap(635) == Gap::LEFT);

    const uint32_t start = UINT32_MAX - 100;
    startWithWalls(monitor, start);
    for (uint32_t offset : {300u, 367u, 434u}) pair(monitor, start + offset, 150, 3001);
    assert(monitor.gap(start + 434) == Gap::RIGHT);
}

static void rejectsHistoryAfterManeuver() {
    Monitor monitor;
    startWithWalls(monitor);
    for (uint32_t now : {300u, 367u, 434u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(434) == Gap::LEFT);
    monitor.invalidate();
    assert(monitor.gap(434) == Gap::UNCONFIRMED);

    // Fresh plausible readings after evasion must not rearm the initial reference.
    for (uint32_t now : {501u, 568u, 635u}) pair(monitor, now, 150, 150);
    for (uint32_t now : {702u, 769u, 836u}) pair(monitor, now, 150, 3001);
    assert(monitor.gap(836) == Gap::UNCONFIRMED);

    // Discarding history during initial acquisition also prevents a later decision.
    monitor.begin(1000);
    pair(monitor, 1010, 150, 150);
    monitor.invalidate();
    for (uint32_t now : {1077u, 1144u, 1211u}) pair(monitor, now, 150, 150);
    for (uint32_t now : {1278u, 1345u, 1412u}) pair(monitor, now, 3001, 150);
    assert(monitor.gap(1412) == Gap::UNCONFIRMED);

    // Starting a new run explicitly restores monitoring.
    startWithWalls(monitor, 2000);
    for (uint32_t now : {2300u, 2367u, 2434u}) pair(monitor, now, 150, 3001);
    assert(monitor.gap(2434) == Gap::RIGHT);
}

int main() {
    confirmsBothDirections();
    rejectsNoiseAndAmbiguity();
    rejectsMissingOrStaleReferences();
    keepsReferenceAndResets();
    handlesThresholdsAndClockWrap();
    rejectsHistoryAfterManeuver();
    puts("OpenDirectionMonitor: all regression checks passed");
}
