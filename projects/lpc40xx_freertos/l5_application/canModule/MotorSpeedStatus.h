#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef enum {
  AXIS_0,
  AXIS_1,
  NUM_OF_AXIS,
} axis_type_e;

typedef enum {
  // Default state at boot up
  MOTOR_AXIS_0,
  MOTOR_AXIS_1,
} motor_type_t;

void MotorSpeedStatus_setMotorCurrentState(motor_type_t motorAxis, uint8_t stateValue);

uint8_t MotorSpeedStatus_getMotorCurrentState(motor_type_t motorAxis);

void motorCalibrationSequence(void);

bool getMotorCalibrationStatus(void);