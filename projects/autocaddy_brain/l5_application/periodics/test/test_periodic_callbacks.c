#include <stdio.h>
#include <string.h>

#include "unity.h"

// Include the Mocks
// - This will not pull the REAL source code of these modules (such as board_io.c)
// - This will auto-generate "Mock" versions based on the header file
#include "Mockble_position.h"
#include "Mockboard_io.h"
#include "Mockcan_bus_initializer.h"
#include "Mockcompass.h"
#include "Mockgeo_logic.h"
#include "Mockgpio.h"
#include "Mockgps.h"

// Include the source we wish to test
#include "periodic_callbacks.h"

void setUp(void) {}

void tearDown(void) {}

void test__periodic_callbacks__initialize(void) {
  can_bus_initializer_ExpectAndReturn(true);
  ble_module_init_Expect();
  compass__init_Expect();
  gps__init_Expect();
  periodic_callbacks__initialize();
}

void test__periodic_callbacks__1Hz(void) {
  gpio_s gpio = {};
  // board_io__get_led0_ExpectAndReturn(gpio);
  // gpio__toggle_Expect(gpio);
  periodic_callbacks__1Hz(0);
}