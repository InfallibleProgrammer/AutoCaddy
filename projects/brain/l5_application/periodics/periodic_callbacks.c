#include "periodic_callbacks.h"
#include "ble_position.h"
#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_module.h"
#include "compass.h"
#include "compass_calibrate.h"
#include "geo_logic.h"
#include "gpio.h"
#include "gps.h"
#include "project.h"
#include <stdio.h>

/******************************************************************************
 * Your board will reset if the periodic function does not return within its deadline
 * For 1Hz, the function must return within 1000ms
 * For 1000Hz, the function must return within 1ms
 */
#define AXIS_0_ID 3u
#define AXIS_1_ID 1u

static coordinate_s cellular_coordinates = {0U};

void periodic_callbacks__initialize(void) {
  // This method is invoked once when the periodic tasks are created
  compass__init();
  gps__init();
  can_bus_initializer();
  ble_position__init();
}

void periodic_callbacks__1Hz(uint32_t callback_count) {
  gpio__toggle(board_io__get_led0());
  gps__setup_command_registers();
  // COMPASS_CAL_1Hz();
}

void periodic_callbacks__10Hz(uint32_t callback_count) {

  gpio__toggle(board_io__get_led1());
  gps__run_once();
  compass__run_once();

  if (ble_position__periodic(&cellular_coordinates)) {
    const dbc_GPS_CURRENT_INFO_s current_RC_location = geo_controller_process_GEO_current_location();
    // printf("phone longitude: %f\n", cellular_coordinates.longitutde);
    // printf("phone latitude: %f\n", cellular_coordinates.latitude);

    dbc_COMPASS_HEADING_DISTANCE_s current_compass_data = {0U};

    if (cellular_coordinates.keyword == LONGITUDE) {
      set_phone_location(cellular_coordinates);
      current_compass_data = determine_compass_heading_and_distance();
    } else if (cellular_coordinates.keyword == STOP) {
      current_compass_data.DESTINATION_HEADING = 0;
      current_compass_data.DISTANCE = 0;
    } else if (cellular_coordinates.keyword == DISTANCE) {
      current_compass_data.DESTINATION_HEADING = cellular_coordinates.bearing;
      current_compass_data.DISTANCE = cellular_coordinates.distance;
    }

    printf("BEARING: %f, DISTANCE: %f\n", current_compass_data.DESTINATION_HEADING, current_compass_data.DISTANCE);

    can__msg_t can_msg = {};
    const dbc_message_header_t heading_and_distance_information =
        dbc_encode_COMPASS_HEADING_DISTANCE(can_msg.data.bytes, &current_compass_data);
    can_msg.msg_id = heading_and_distance_information.message_id;
    can_msg.frame_fields.data_len = heading_and_distance_information.message_dlc;
    can__tx(can1, &can_msg, 0);
  }
  // Add your code here
}

void periodic_callbacks__100Hz(uint32_t callback_count) {
  // Add your code here
  // add logic to send coordinate to queue or flag other periodic to process coordinate
  // const bool ble_data_received = ble_module_init_periodic(&cellular_coordinates);
  // if (ble_data_received) {
  // TODO: set gps data
  //}
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