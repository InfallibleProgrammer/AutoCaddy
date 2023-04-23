#include <stdio.h>
#include <string.h>

#include "compass.h"
#include "compass_calibrate.h"

#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"

#define CALIBRATION_COUNTER (60U)

// Typedefs
typedef enum {
  calibrate_erase = 0,
  calibrate_start,
  calibrate_magnetometer,
  calibrate_accelerometer,
  calibrate_gyro,
  calibrate_store_profile,
  calibrate_hold
} calibration_state_E;

typedef struct {
  calibration_state_E calibration_state;
  bool calibration_started;
  uint32_t counter;
} calibration_vars_S;

// Private variables
static compass_t compass = {0};
calibration_vars_S calibration_var = {0};
static bool compass_isCalibrated = false;
static uint32_t startup_delay = 0;

// Private functions
static void begin_calibrate_sequence(void);
static void erase_calibration_profile(void);
static bool store_profile(void);

// Public function definitions
void COMPASS_CAL_calibrate(void) {
  bool success = false;
  char status = 0;
  switch (calibration_var.calibration_state) {
  case calibrate_erase:
    erase_calibration_profile();
    calibration_var.calibration_state = calibrate_gyro;
    break;

  case calibrate_gyro:
    if (!calibration_var.calibration_started) {
      begin_calibrate_sequence();
      i2c__write_single(I2C__2, 0xC0, 0x0, 0x83);
      calibration_var.calibration_started = true;
      printf("Calibrating gyrometer\n");
    } else {
      if (calibration_var.counter > CALIBRATION_COUNTER) {
        printf("Calibrating gyrometer done\n");
        begin_calibrate_sequence();
        i2c__write_single(I2C__2, 0xC0, 0x0, 0x80);
        calibration_var.counter = 0;
        calibration_var.calibration_started = false;
        calibration_var.calibration_state = calibrate_magnetometer;
      } else {
        calibration_var.counter++;
      }
    }
    break;

  case calibrate_magnetometer:
    if (!calibration_var.calibration_started) {
      begin_calibrate_sequence();
      i2c__write_single(I2C__2, 0xC0, 0x0, 0x81);
      calibration_var.calibration_started = true;
      printf("calibrating magnetometer\n");
    } else {
      if (calibration_var.counter > CALIBRATION_COUNTER) {
        printf("Calibrating magnetometer done\n");
        begin_calibrate_sequence();
        i2c__write_single(I2C__2, 0xC0, 0x0, 0x80);
        calibration_var.counter = 0;
        calibration_var.calibration_started = false;
        calibration_var.calibration_state = calibrate_accelerometer;
      } else {
        calibration_var.counter++;
      }
    }
    break;

  case calibrate_accelerometer:
    if (!calibration_var.calibration_started) {
      begin_calibrate_sequence();
      i2c__write_single(I2C__2, 0xC0, 0x0, 0x82);
      calibration_var.calibration_started = true;
      printf("calibrating accelerometer\n");
    } else {
      if (calibration_var.counter > CALIBRATION_COUNTER) {
        printf("Calibrating accelerometer done\n");
        begin_calibrate_sequence();
        i2c__write_single(I2C__2, 0xC0, 0x0, 0x80);
        calibration_var.counter = 0;
        calibration_var.calibration_started = false;
        calibration_var.calibration_state = calibrate_store_profile;
      } else {
        calibration_var.counter++;
      }
    }
    break;

  case calibrate_store_profile:
    store_profile();
    calibration_var.calibration_state = calibrate_hold;
    break;

  case calibrate_hold:
    printf("Holding");
    break;

  default:
    break;
  }
}

void COMPASS_CAL_1Hz(void) {
  if (startup_delay < 5) {
    printf("startup delay:%ld\n", startup_delay);
    startup_delay++;
  } else {
    if (compass_isCalibrated) {
      compass__run_once();
    } else {
      COMPASS_CAL_calibrate();
    }
  }
}

static void begin_calibrate_sequence(void) {
  bool ret = false;
  const char calibration_bytes[] = {0x98, 0x95, 0x99};

  printf("Beginning calibration sequence\n");
  for (uint8_t i = 0; i < 3; i++) {
    i2c__write_single(I2C__2, 0xC0, 0x00, calibration_bytes[i]);
    delay__ms(20);
  }
}

static void erase_calibration_profile(void) {
  const char calibration_bytes[] = {0xE0, 0xE5, 0xE2};

  printf("Erase calibration sequence\n");
  for (uint8_t i = 0; i < 3; i++) {
    i2c__write_single(I2C__2, 0xC0, 0x00, calibration_bytes[i]);
    delay__ms(20);
  }
  delay__ms(300);
}

static bool store_profile(void) {
  bool ret = false;

  char calibration_status = i2c__read_single(I2C__2, 0xC0, 0x1E);
  printf("calibration status %x", calibration_status);
  if ((calibration_status & (1U << 7)) && (calibration_status & (1U << 6))) {
    const char calibration_bytes[] = {0xF0, 0xF5, 0xF6};

    printf("Storing profile sequence\n");
    for (uint8_t i = 0; i < 3; i++) {
      char status = 0;
      i2c__write_single(I2C__2, 0xC0, 0x00, calibration_bytes[i]);
      delay__ms(20);
    }
    ret = true;
  }
  if (ret == true) {
    compass_isCalibrated = true;
  }
  return ret;
}