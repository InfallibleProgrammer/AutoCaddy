#include "load_sensor.h"
#include "delay.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>

#define GAIN_SETTING 3

static int32_t offset = 0;

static gpio_s load_sensor__clk_pin = {
    .port_number = 0,
    .pin_number = 7,
};

static gpio_s load_sensor__data_pin = {
    .port_number = 0,
    .pin_number = 9,
};

static uint8_t load_sensor__calculate_filler_byte(int32_t reading) { return (reading & (1 << 23)) ? 0xFF : 0x00; }

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

static bool load_sensor__read_bytes(int32_t *sensor_value) {
  bool weight_read = false;
  *sensor_value = 0;
  if (load_sensor__is_ready()) {
    weight_read = true;
    gpio__reset(load_sensor__clk_pin);
    for (uint8_t count = 1; count <= 24; ++count) {
      gpio__set(load_sensor__clk_pin);
      delay__us(1);
      bool val = gpio__get(load_sensor__data_pin);
      if (val) {
        *sensor_value |= (1 << (24 - count));
      }
      gpio__reset(load_sensor__clk_pin);
      delay__us(1);
    }
    // for (uint8_t pulse_count = 0; pulse_count < GAIN_SETTING; ++pulse_count) {
    //   gpio__set(load_sensor__clk_pin);
    //   delay__us(1);
    //   gpio__reset(load_sensor__clk_pin);
    //   delay__us(1);
    // }
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
  // wait for sensor to be ready
  while (!load_sensor__is_ready())
    ;
  int32_t sensor_offset = 0;
  // set initial gain
  gpio__reset(load_sensor__clk_pin);
  for (uint8_t gain_pulse = 0U; gain_pulse < GAIN_SETTING; ++gain_pulse) {
    delay__us(1);
    gpio__set(load_sensor__clk_pin);
    delay__us(1);
    gpio__reset(load_sensor__clk_pin);
  }
  (void)load_sensor__read(&sensor_offset);
  const uint8_t filler_byte = load_sensor__calculate_filler_byte(sensor_offset);
  offset = sensor_offset;
  printf("offset: %i\n", offset);
}

bool load_sensor__read(int32_t *sensor_reading) {
  bool valid_reading = false;
  if (load_sensor__read_bytes(sensor_reading)) {
    valid_reading = true;
    const uint8_t filler_byte = load_sensor__calculate_filler_byte(*sensor_reading);
    *sensor_reading -= offset;
  }
  return valid_reading;
}