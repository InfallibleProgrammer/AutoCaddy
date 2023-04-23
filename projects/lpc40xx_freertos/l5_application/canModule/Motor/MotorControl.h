#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef enum {
  MOTOR_0,
  MOTOR_1,
  NUM_OF_MOTORS,
} motor_axis_e;

typedef enum {
  INITIAL_CALIBRATION_STATE = 0,
  ENCODER_CALIBRATION_STAGE = 1,
  ENCODER_CALIBRATED = 2,
} motor_calibration_state_e;

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
  motor_calibration_state_e calibrationState;
  uint8_t axisCanID;
  axis_state_e motorState_e;
} motor_parameters_e;

void MotorControl_sendVelocityData(motor_axis_e motorSide);

void MotorControl_updateVelocityData(float inputVelocity, motor_axis_e motorSide);

void MotorControl_calibrateMotors(motor_axis_e motorSide);

void MotorControl_motorCalibrationSequence(void);

uint8_t MotorControl_getMotorCANID(motor_axis_e motorSide);

bool MotorControl_isMotorCalibrated(motor_axis_e motorSide);

void MotorControl_setState(motor_axis_e motorSide, axis_state_e stateValue);

void MotorControl_init(void);