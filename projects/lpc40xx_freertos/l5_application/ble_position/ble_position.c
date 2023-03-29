#include <string.h>

#include "ble_position.h"
#include "math.h"
#include "uart3_init.h"

static const size_t first_index = 0U;
static const size_t second_index = 1U;
static size_t index = 0U;
static char coordinate_buffer[512U] = {0U};
static keyword_e parsed_keyword = KEYWORD_COUNT;

static keyword_e ble_position__parse_keyword(char *message_buffer, size_t message_size) {
  keyword_e keyword = STOP;

  if (message_buffer[first_index] == 'L') {
    keyword = (message_buffer[second_index] == 'o') ? LONGITUDE : LATITUDE;
  }

  return keyword;
}

static long double ble_position__parse_latitude_longitude(char *message_buffer, size_t message_size) {
  const bool sign = (message_buffer[first_index] == '-') ? false : true;
  bool period_found = false;
  long double value = 0;
  size_t first_index = (sign) ? 0U : 1U;
  size_t count_after_period = 1U;
  for (first_index; first_index < message_size; ++first_index) {
    if (period_found) {
      const long double converted_char =
          (long double)((double)message_buffer[first_index] - '0') / (long double)(powl(10.0, count_after_period));
      value += converted_char;
      count_after_period++;
    } else {
      if (message_buffer[first_index] != '.') {
        value = (value * 10) + (message_buffer[first_index] - '0');
      } else {
        period_found = true;
      }
    }
  }
  return (sign) ? value : (-value);
}

static bool ble_position__handle_latitude_longitude(long double parsed_coordinate, coordinate_s *coordinate) {
  bool coordinate_complete = false;
  switch (parsed_keyword) {
  case LONGITUDE:
    coordinate->longitutde = parsed_coordinate;
    break;
  case LATITUDE:
    coordinate->latitude = parsed_coordinate;
    coordinate_complete = true;
    break;
  case STOP:
  case KEYWORD_COUNT:
  default:
    break;
  }
  return coordinate_complete;
}

void ble_position__init(void) { uart3_init(); }

bool ble_position__periodic(coordinate_s *cooridnate) {
  bool coordinate_parsed = false;
  if (uart__get(UART__3, &coordinate_buffer[index], 0U)) {
    if (coordinate_buffer[index] == ':') {
      // parse keyword
      parsed_keyword = ble_position__parse_keyword(coordinate_buffer, index);
      memset(coordinate_buffer, 0U, sizeof(coordinate_buffer));
    } else if (coordinate_buffer[index] == ',') {
      memset(coordinate_buffer, 0U, sizeof(coordinate_buffer));
      index = 0U;
    } else {
      index++;
    }
  }
  return coordinate_parsed;
}