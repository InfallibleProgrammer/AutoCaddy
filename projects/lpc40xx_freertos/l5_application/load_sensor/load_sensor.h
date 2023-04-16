#pragma once

#include <stdbool.h>
#include <stdint.h>

void load_sensor__init(void);
bool load_sensor__is_ready(void);
void load_sensor__calibrate(void);
int32_t load_sensor__read(void);