#include "MotorLogic.h"
#include "Interpolation.h"
#include "MotorControl.h"

#define PERCENTAGE_UNIT_PER_DEGREE .005555f // 100 percent/ 180 degree max / 100 percent to decimal

static float optimalMotorSpeed = 0;
interpolation_point_xy distance_lookupTable[5] = {
    {
        .x = 0,
        .y = 0,
    },
    {
        .x = 3.0f,
        .y = 0.15,
    },
    {
        .x = 7.0f,
        .y = 1.5,
    },
    {
        .x = 10,
        .y = 2.5,
    },
    {
        .x = 17,
        .y = 3,
    },

};

void updateMotorValues(float bearing, float distance) {
  optimalMotorSpeed = linear_interp(distance, &distance_lookupTable, 5);
  static float motorSpeed;
  //
  // left motor changes
  if (bearing >= 0 && bearing <= 180) {
    motorSpeed = bearing * PERCENTAGE_UNIT_PER_DEGREE * optimalMotorSpeed;
    MotorControl_updateVelocityData(MOTOR_0, -motorSpeed);
    MotorControl_updateVelocityData(MOTOR_1, optimalMotorSpeed);
  }
  // right motor changes
  else if (bearing > 180 && bearing <= 360) {
    motorSpeed = (bearing - 180.0) * PERCENTAGE_UNIT_PER_DEGREE * optimalMotorSpeed;
    MotorControl_updateVelocityData(MOTOR_1, motorSpeed);
    MotorControl_updateVelocityData(MOTOR_0, -optimalMotorSpeed);
  }
}
