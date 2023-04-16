#include "load_sensor.h"
#include "delay.h"
#include "gpio.h"
#include "string.h"

static uint32_t offset = 0;
static const uint8_t number_of_reads = 3;

static gpio_s load_sensor__clk_pin = {
    .port_number = 0,
    .pin_number = 7,
};

static gpio_s load_sensor__data_pin = {
    .port_number = 0,
    .pin_number = 9,
};

static uint8_t load_sensor__calculate_filler_byte(uint8_t byte) { return (byte & 0x80) ? 0xFF : 0x00; }

static uint8_t load_sensor__read_byte(void) {
  uint8_t value = 0U;
  gpio__reset(load_sensor__clk_pin);
  delay__us(1);
  for (uint8_t count = 0U; count < 8; ++count) {
    gpio__set(load_sensor__clk_pin);
    delay__us(1);
    bool pin_val = gpio__get(load_sensor__data_pin);
    if (pin_val) {
      value |= 1 << (7 - count);
    }
    gpio__reset(load_sensor__clk_pin);
    delay__us(1);
  }
  return value;
}

static bool load_sensor__read_bytes(uint8_t *byte_arr) {
  bool weight_read = false;
  memset(byte_arr, 0, 3); // set byte arr to zero.
  if (load_sensor__is_ready()) {
    weight_read = true;
    for (uint8_t count = 1; count <= number_of_reads; ++count) {
      byte_arr[number_of_reads - count] = load_sensor__read_byte();
    }
  }
  return weight_read;
}

bool load_sensor__is_ready(void) { return (gpio__get(load_sensor__data_pin)) ? false : true; }

void load_sensor__init(void) {
  (void)gpio__construct_as_output(GPIO__PORT_0, 7);
  (void)gpio__construct_as_input(GPIO__PORT_0, 9);
  gpio__reset(load_sensor__clk_pin);
  printf("calibrating..\n");
  load_sensor__calibrate();
}

void load_sensor__calibrate(void) {
  // calibrate sensor according to offset to get 'zero' weight
  uint8_t byte_arr[3U] = {0U};
  // wait for sensor to be ready
  while (!load_sensor__is_ready())
    ;
  const uint8_t filler_byte = load_sensor__calculate_filler_byte(byte_arr[2]);
  offset = 0U;
  offset = (filler_byte << 24) | (byte_arr[2] << 16) | (byte_arr[1] << 8) | (byte_arr[0]);
  printf("offset: %lu\n", offset);
}

int32_t load_sensor__read(void) {
  uint8_t byte_arr[3U] = {0U};
  int32_t value = 0;
  if (load_sensor__read_bytes(byte_arr)) {
    const uint8_t filler_byte = load_sensor__calculate_filler_byte(byte_arr[2]);
    value =
        (int8_t)(filler_byte << 24) | (int8_t)(byte_arr[2] << 16) | (int8_t)(byte_arr[1] << 8) | (int8_t)(byte_arr[0]);
    value -= (int32_t)offset;
  }
  return value;
}