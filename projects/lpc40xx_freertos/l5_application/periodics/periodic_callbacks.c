#include "periodic_callbacks.h"

#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_module.h"
#include "gpio.h"

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
#define AXIS_0_ID 3u
#define AXIS_1_ID 1u

void periodic_callbacks__initialize(void) {
  // This method is invoked once when the periodic tasks are created
  can_bus_initializer();
  MotorControl_init();
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  // gpio__toggle(board_io__get_led0());
  can_bus_handler__process_all_received_messages();
  periodic_callbacks_1Hz_Velocity();
  MotorControl_motorCalibrationSequence();
  periodic_callbacks_1Hz_sendspeed();
  // Add your code here
}

void periodic_callbacks__10Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led1());
  updateMotorValues(); // update values for the motors
  // Add your code here
}
void periodic_callbacks__100Hz(uint32_t callback_count) {
  // Add your code here
  // add logic to send coordinate to queue or flag other periodic to process coordinate
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