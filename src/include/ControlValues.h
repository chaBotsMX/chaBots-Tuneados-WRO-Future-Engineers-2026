#pragma once


//Constants 
#define MAX_VALID_DISTANCE 3000


//First lap open round

struct MotionTaskConfig {
    // Same units and field order as Move::setTask.
    float distanceCm;
    float maxSpeedCmS;
    float accelerationCmS2;
    float decelerationCmS2;
    float initialSpeedCmS;
    float finalSpeedCmS;
};

struct SpeedControlConfig {
    // ki is retained for a future integral controller; it is not applied yet.
    float targetSpeedCmS;
    float kp;
    float ki;
};

#define FIRST_GO_STRAIGHT_DISTANCE 70
#define FIRST_GO_STRAIGHT_SPEED 50
#define FIRST_GO_STRAIGHT_ACCEL_DECCEL 30


constexpr MotionTaskConfig OPEN_INITIAL_STRAIGHT_PROFILE = {
    95.0f, 50.0f, 30.0f, 30.0f, 0.0f, 0.0f
};

constexpr MotionTaskConfig OPEN_FOLLOW_WALL_PROFILE = {
    110.0f, 150.0f, 120.0f, 120.0f, 0.0f, 0.0f
};

constexpr MotionTaskConfig OPEN_ENDING_PROFILE = {
    40.0f, 30.0f, 60.0f, 60.0f, 0.0f, 0.0f
};

constexpr SpeedControlConfig OPEN_CRUISE_SPEED_CONTROL = {
    120.0f, 1.0f, 0.1f
};

#define EDGING_PWM -70
#define EDGING_TARGET_DISTANCE 300

#define INITIAL_REVERSE_TIME 1000
#define INITIAL_REVERSE_PWM 50

// follow until edge

#define CRUISE_MIN_DISTANCE_MM 600
#define TURN_MIN_DISTANCE_MM 300
#define TURN_MAX_DISTANCE_MM 700
#define INNER_WALL_MIN_DISTANCE_TO_TURN_MM 3000
#define INNER_WALL_MAX_DISTANCE_TO_CRUISE_MM 3000
#define FOLLOW_WALL_RECOVERY_PWM 200


// Open Turn controls

#define OPEN_TURN_ERROR_THRESHOLD_DEG 5
#define OPEN_TURN_PWM -200
#define OPEN_TURN_STEERING_GAIN 0.5f


// Tangencial Evasion Controls
// These distances use OpenMV pixels; the _MM suffix is retained for compatibility.
#define TAN_EVASION_SECURITY_RADIUS_MM 115 // Minimum distance to the obstacle to start evasion
#define TAN_EVASION_ACTIVATION_DISTANCE_MM 200 // Distance to activate tangent evasion maneuver
#define TAN_EVASION_FULL_EVASION_DISTANCE_MM 160 // Distance to activate full tangent evasion maneuver
#define TAN_EVASION_ORIENTATION_GAIN 2.0f // Gain for orientation error in tangent evasion
#define TAN_EVASION_EVASION_GAIN 4.0f // Gain for evasion
#define TAN_EVASION_KP 1.0f // Proportional gain for PD
#define TAN_EVASION_KD 0.02f // Derivative gain for PD
#define TAN_EVASION_DERIVATIVE_FILTER 0.20f // Derivative filter

// Obstacle Controls
#define OBSTACLES_PWM_DRIVE -80 // Standar Drive PWM
#define OBSTACLES_RECOVERY_TIME_MS 350 // Recovery time when to close to obstacle
#define SIDE_WALLS_ACTIVATION_DISTANCE_MM 200 // Distance to activate tangent evasion maneuver
#define NO_OBSTACLE_IMU_GAIN 1.0f
#define OBSTACLE_CLOSE_RECOVERY_DISTANCE_MM 80 // Distance to activate recovery maneuver when too close to obstacle

// SIDE WALLS CONTROLS
#define SIDE_WALLS_ACTIVATION_DISTANCE_MM 200 // Distance to activate tangent evasion
#define SIDE_WALLS_STEERING_ANGLE_DEG 30 // Steering angle to avoid side walls


// BLUE LINE CONTROLS

#define BLUE_LINE_STABLE_TIME_MS 100 // Minimum time that the blue line must be detected to trigger a turn
#define BLUE_LINE_REARM_DISTANCE_MM 1000.0f // MM that the robot have to travel before trigger other turn


// Vision constants

#define VISION_WIDTH 320
#define VISION_HEIGHT 240

// final lap

#define PARKING_DISTANCE_MM 1500
