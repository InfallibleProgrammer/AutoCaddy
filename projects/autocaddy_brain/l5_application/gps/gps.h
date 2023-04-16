#pragma once

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* External Includes */
#include "project.h"
/* Module Includes */

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

typedef struct {
  float latitude;
  float longitude;
  float heading;
} gps_coordinates_t;

/***********************************************************************************************************************
 *
 *                                     F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

void gps__init(void);
void gps__run_once(void);
void print__gps_coordinates(void);
bool gps_is_fixed(void);
void gps__setup_command_registers(void);
gps_coordinates_t gps__get_coordinates(void);