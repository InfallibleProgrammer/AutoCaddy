#pragma once
#include "gpio.h"
#include "ssp2.h"

typedef enum {
  ACTIVE_LOW,
  ACTIVE_HIGH,
} cs_active_level_e;

typedef struct {
  gpio_s chipselect;
  cs_active_level_e activeLevel;
  uint32_t clockRate;
} gps_spi_e;

void GPS_init(gps_spi_e gps_spi_configuration);
