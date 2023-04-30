#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t bearing;
  bool calibrated;
} compass_t;

void COMPASS_CAL_1Hz(void);
void COMPASS_CAL_calibrate(void);
bool COMPASS_CAL_calibrate_gyro(void);
bool COMPASS_CAL_calibrate_accelerometer(void);
bool COMPASS_CAL_calibrate_magnetometer(void);
