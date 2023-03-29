#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef enum {
  MOTOR_0,
  MOTOR_1,
  NUM_OF_MOTORS,
} motor_axis_e;

typedef enum {
  UNDEFINED = 0,
  IDLE = 1,
  STARTUP_SEQUENCE = 2,
  FULL_CALIBRATION_SEQUENCE = 3,
  MOTOR_CALIBRATION = 4,
  ENCODER_INDEX_SEARCH = 6,
  ENCODER_OFFSET_CALIBRATION = 7,
  CLOSED_LOOP_CONTROL = 8,
  LOCKIN_SPIN = 9,
  ENCODER_DIR_FIND = 10,
} axis_state_e;

typedef struct {
  uint8_t motorAxisType;
  float inputSpeed;
  float inputTorque;
  bool isCalibrated;
  bool calibrationState;
  uint8_t axisCanID;
  uint8_t motorState_e;
} motor_parameters_e;

void MotorControl_sendVelocityData(motor_axis_e motorSide);

void MotorControl_updateVelocityData(float inputVelocity, motor_axis_e motorSide);

void MotorControl_calibrateMotors(motor_axis_e motorSide);