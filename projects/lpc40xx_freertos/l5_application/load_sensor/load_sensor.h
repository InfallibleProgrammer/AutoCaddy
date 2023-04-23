#pragma once

#include <stdbool.h>
#include <stdint.h>

void load_sensor__init(void);
bool load_sensor__is_ready(void);
void load_sensor__calibrate(void);
bool load_sensor__read(int32_t *sensor_reading);