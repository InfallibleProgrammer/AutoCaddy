#include <stdio.h>
#include <string.h>

#include "compass.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "uart_printf.h"

// Defines

// Private variables
static float heading = 0;

// Public function definitions
void compass__init(void) { heading = 0; }

void compass__run_once(void) {
  uint8_t data[2] = {0U};
  uint8_t items_to_read = 2;
  i2c__read_slave_data(I2C__2, 0xC0, 0x02, data, items_to_read);
  heading = ((0xFF00 & ((uint8_t)data[0] << 8)) | (0x00FF & ((uint8_t)data[1]))) / 10.0;
  printf("Heading %f\n", heading);
}

float compass__run_once_get_heading(void) { return heading; }