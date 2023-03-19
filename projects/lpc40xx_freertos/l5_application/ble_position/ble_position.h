#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
  LONGITUDE = 0,
  LATITUDE,
  STOP,
} keyword_e;

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

keyword_e parse_keyword(char *message_buffer, size_t message_size);
long double parse_latitude_longitude(char *message_buffer, size_t message_size);