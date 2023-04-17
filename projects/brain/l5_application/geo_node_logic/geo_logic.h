#pragma once

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */

/* External Includes */
#include "project.h"
/* Module Includes */
#include "ble_position.h"

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/
/*
typedef struct {
  bool is_data_pending;
  dbc_DBG_CONFIRM_RECEIVED_DESTINATION_s status_values;

} confirm_received_destination_s;
*/
/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                     F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/
dbc_COMPASS_HEADING_DISTANCE_s determine_compass_heading_and_distance(void);
dbc_GPS_CURRENT_INFO_s geo_controller_process_GEO_current_location(void);
// dbc_GPS_CURRENT_DESTINATIONS_DATA_s determine_destination_location(void);
void set_phone_location(coordinate_s destination_coordinate);

// LEGACY CODE
// void write_to_sd_card(void);
// debug functions located below
// void processed_compass_gps_data(void);
// confirm_received_destination_s confirm_last_added_destination(void);
// dbc_GPS_COMPASS_STATUS_s check_GPS_Compass_Status(void);
// dbc_DBG_GPS_COMPASS_LOCK_LED_CHECK_s check_GPS_COMPASS_LED(void);
// int get_distance(void);