#include "ble_position.h"
#include "math.h"

static const size_t first_index = 0U;
static const size_t second_index = 1U;

keyword_e parse_keyword(char *message_buffer, size_t message_size) {
  keyword_e keyword = STOP;

  if (message_buffer[first_index] == 'L') {
    keyword = (message_buffer[second_index] == 'o') ? LONGITUDE : LATITUDE;
  }

  return keyword;
}

long double parse_latitude_longitude(char *message_buffer, size_t message_size) {
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