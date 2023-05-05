#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  LONGITUDE = 0,
  LATITUDE,
  DISTANCE,
  BEARING,
  STOP,
  KEYWORD_COUNT,
} keyword_e;

typedef struct {
  long double latitude;
  long double longitutde;
  long double distance;
  long double bearing;
  keyword_e keyword;
} coordinate_s;

void ble_position__init(void);
bool ble_position__periodic(coordinate_s *cooridnate);
