#include "periodic_callbacks.h"

#include "ble_position.h"
#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_module.h"
#include "gpio.h"
#include "load_sensor.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
#define AXIS_0_ID 3u
#define AXIS_1_ID 1u

static coordinate_s cellular_coordinates = {0U};
static int32_t weight_reading = 0;

void periodic_callbacks__initialize(void) {
  // This method is invoked once when the periodic tasks are created
  can_bus_initializer();
  // initCanMotorPackets(AXIS_0_ID, AXIS_1_ID);
  ble_module_init();
  load_sensor__init();
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  // gpio__toggle(board_io__get_led0());

  periodic_callbacks_1Hz_Velocity();
  can_bus_handler__process_all_received_messages();
  if (load_sensor__is_ready()) {
    int32_t sensor_value = load_sensor__read();
    printf("weight: %li\n", sensor_value);
  }
  // Add your code here
}

void periodic_callbacks__10Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led1());
  // Add your code here
}
void periodic_callbacks__100Hz(uint32_t callback_count) {
  // Add your code here
  // add logic to send coordinate to queue or flag other periodic to process coordinate
  (void)ble_module_init_periodic(&cellular_coordinates);
}

/**
 * @warning
 * This is a very fast 1ms task and care must be taken to use this
 * This may be disabled based on intialization of periodic_scheduler__initialize()
 */
void periodic_callbacks__1000Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led3());
  // Add your code here
}