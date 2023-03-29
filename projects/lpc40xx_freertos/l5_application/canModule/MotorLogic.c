#include "MotorLogic.h"
#include "Interpolation.h"

float optimalMotorSpeed = 0;
interpolation_point_xy distance_lookupTable[5] = {
    {
        .x = 0,
        .y = 0,
    },
    {
        .x = 10,
        .y = 1.5,
    },
    {
        .x = 20,
        .y = 2.5,
    },
    {
        .x = 30,
        .y = 3.5,
    },
    {
        .x = 50,
        .y = 5,
    },

};

void updateMotorValues(float bearing, float distance) {
  optimalMotorSpeed = linear_interp(distance, &distance_lookupTable, 5);
  //
}