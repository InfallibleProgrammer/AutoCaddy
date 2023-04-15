#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  LONGITUDE = 0,
  LATITUDE,
  STOP,
  KEYWORD_COUNT,
} keyword_e;

typedef struct {
  long double latitude;
  long double longitutde;
} coordinate_s;

//  if (uart__get(UART__3, &message_buffer[index], 1000U)) {
//       if (message_buffer[index] == ':') {
//         // parse keyword
//         keyword_e keyword = parse_keyword(message_buffer, index);
//         memset(message_buffer, 0, sizeof(message_buffer));
//         index = 0U;
//       } else if (message_buffer[index] == ',') {
//         // parse lat/ long
//         const long double value = parse_latitude_longitude(message_buffer, index);
//         memset(message_buffer, 0, sizeof(message_buffer));
//         index = 0U;
//       } else {
//         index++;
//       }

void ble_position__init(void);
bool ble_position__periodic(coordinate_s *cooridnate);
