/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Main Module Header */
#include "geo_logic.h"
/* Standard Includes */
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* External Includes */

#include "ble_position.h"
#include "compass.h"
#include "ff.h"
#include "gpio.h"
#include "gps.h"
#include "project.h"

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

#define PI 3.14159265358979323846
/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/
// logic add destination
// determine shortest distance
// once we have arrived, set destination as invalid
static dbc_GPS_CURRENT_INFO_s current_RC_CAR_location = {0U};

static dbc_COMPASS_HEADING_DISTANCE_s current_destination_information = {0U};

static gps_coordinates_t rc_car, destination;
// static coordinate_s phone_location = {.latitude = 37.40183, .longitutde = -121.88074};
static coordinate_s phone_location = {0};

static bool phone_location_received;
// static double distance;
/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/
// static float calculate_distance_rc_car_to_destination(void) {
//   return sqrt(pow(current_RC_CAR_location.GPS_CURRENT_LONG - current_destination.CURRENT_DEST_LONGITUDE, 2) +
//               pow(current_RC_CAR_location.GPS_CURRENT_LAT - current_destination.CURRENT_DEST_LATITUDE, 2));
// }

/*
static float calculate_distance_rc_car_to_destination_in_meters(gps_coordinates_t origin,
                                                                gps_coordinates_t destination) {
  const int Earth_Radius_in_km = 6371;
  const float delta_lat = (origin.latitude - destination.latitude) * (PI / 180);
  const float delta_long = (origin.longitude - destination.longitude) * (PI / 180);
  float a = pow(sinf(delta_lat / 2), 2) +
            cosf(destination.latitude * (PI / 180)) * cosf(origin.latitude * (PI / 180)) * pow(sin(delta_long / 2), 2);
  float c = 2 * atan2f(sqrt(a), sqrt(1 - a));
  return (float)(Earth_Radius_in_km * c * 1000);
}
*/  // OLD IMPLEMENTATION

static long double calculate_distance_rc_car_to_destination_in_meters(void) {
  const int Earth_Radius_in_km = 6371;
  const long double delta_lat = (current_RC_CAR_location.GPS_CURRENT_LAT - phone_location.latitude) * (PI / 180.0f);
  const long double delta_long = (current_RC_CAR_location.GPS_CURRENT_LONG - phone_location.longitutde) * (PI / 180.0f);
  long double a = pow(sin(delta_lat / 2), 2) + cos(phone_location.latitude * (PI / 180.0f)) *
                                                   cos(current_RC_CAR_location.GPS_CURRENT_LAT * (PI / 180.0f)) *
                                                   pow(sin(delta_long / 2), 2);
  long double c = 2 * atan2f(sqrt(a), sqrt(1 - a));
  long double distance = (Earth_Radius_in_km * c * 1000);
  printf("Distance: %f\n", distance);
  return (Earth_Radius_in_km * c * 1000);
}

/*
static float calculate_distance(uint8_t array_index) {
  return sqrt(
      pow(current_RC_CAR_location.GPS_CURRENT_LONG - internal_geo_destination_data[array_index].DEST_LONGITUDE, 2) +
      pow(current_RC_CAR_location.GPS_CURRENT_LAT - internal_geo_destination_data[array_index].DEST_LATITUDE, 2));
}
*/

static float calculate_destination_bearing(void) {
  // setting variables
  const float difference_in_longitude = (float)phone_location.longitutde - current_RC_CAR_location.GPS_CURRENT_LONG;
  const float destination_latitude = phone_location.latitude;
  const float source_latitude = current_RC_CAR_location.GPS_CURRENT_LAT;

  // Actual calculations
  float x = cos(destination_latitude) * sin(difference_in_longitude);
  float y = cos(source_latitude) * sin(destination_latitude) -
            sin(source_latitude) * cos(destination_latitude) * cos(difference_in_longitude);
  float bearing = (float)(atan2(x, y));
  if (bearing < 0) {
    return ((bearing * (180.0f / (float)PI)) + 360);
  }
  // printf("BEARING IN RADIANS: %f\n", (bearing * (180.0f / (float)PI)));
  return (bearing * (180.0f / (float)PI));
}

/***********************************************************************************************************************
 *
 *                                          P U B L I C   F U N C T I O N S
 *
 **********************************************************************************************************************/
void set_phone_location(coordinate_s destination_coordinate) {
  phone_location = destination_coordinate;
  phone_location_received = true;
};

dbc_GPS_CURRENT_INFO_s geo_controller_process_GEO_current_location(void) {
  gps_coordinates_t gps_peripheral_data;
  gps_peripheral_data = gps__get_coordinates();
  current_RC_CAR_location.GPS_CURRENT_LONG = gps_peripheral_data.longitude;
  current_RC_CAR_location.GPS_CURRENT_LAT = gps_peripheral_data.latitude;
  return current_RC_CAR_location;
}

dbc_COMPASS_HEADING_DISTANCE_s determine_compass_heading_and_distance(void) {
  gps_coordinates_t gps_peripheral_data;
  gps_peripheral_data = gps__get_coordinates();
  current_destination_information.CURRENT_HEADING = compass__run_once_get_heading();
  current_destination_information.DESTINATION_HEADING = calculate_destination_bearing();
  // current_destination_information.DESTINATION_HEADING = get_current_bearing();
  current_destination_information.DISTANCE = calculate_distance_rc_car_to_destination_in_meters();
  // printf("Destination bearing: %f\n", current_destination_information.DESTINATION_HEADING);
  // printf("Destination distance in meters: %f\n", current_destination_information.DISTANCE);
  return current_destination_information;
}

// DEBUG functions
/*
dbc_GPS_COMPASS_STATUS_s check_GPS_Compass_Status(void) {
  dbc_GPS_COMPASS_STATUS_s status_values = {.COMPASS_LOCK_VALID = 0, .GPS_LOCK_VALID = 0};
  status_values.COMPASS_LOCK_VALID = compass__is_data_valid();
  status_values.GPS_LOCK_VALID = gps_is_fixed();
  return status_values;
}
*/
