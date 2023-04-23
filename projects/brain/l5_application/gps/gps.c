/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Main Module Header */
#include "gps.h"
/* Standard Includes */
#include <math.h>
#include <stdio.h>
#include <string.h>
/* External Includes */
#include "FreeRTOS.h"
#include "board_io.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "line_buffer.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include "uart.h"

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

/***********************************************************************************************************************
 *
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/

static char line_buffer[256];
static line_buffer_s line;

static uart_e gps_uart = UART__2;
static gps_coordinates_t parsed_coordinates;
static gpio_s gps_fix = {0, 6};
// static bool gps_lock_status;

static bool is_setup = false;
static bool baud_changed = false;

/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/

static void gps__transfer_data_from_uart_driver_to_line_buffer(void) {
  char byte;
  const uint32_t zero_timeout = 0;

  while (uart__get(gps_uart, &byte, zero_timeout)) {
    line_buffer__add_byte(&line, byte);
  }
}

static void gps__nema(const char *msg, size_t msg_size) {
  static const uint32_t delay_for_startup_sequence_avoidance = 10;
  delay__ms(delay_for_startup_sequence_avoidance);
  for (int i = 0; i < msg_size - 1; i++) {
    uart__put(gps_uart, msg[i], 0);
  }
  delay__ms(delay_for_startup_sequence_avoidance);
}

static void gps_convert_string_to_latitude() {
  int dd = (int)parsed_coordinates.latitude / 100;
  float mm = fmod(parsed_coordinates.latitude, 100);
  parsed_coordinates.latitude = (float)dd + (mm / 60);
}

static void gps_convert_string_to_longitude() {
  int ddd = (int)parsed_coordinates.longitude / 100;
  float mm = fmod(parsed_coordinates.longitude, 100);
  parsed_coordinates.longitude = (float)ddd + (mm / 60);
}

static bool gps__check_if__complete_GPGGA_string(char *uart_data) {
  int count = 0;
  char c = ',';
  printf("%s\n", uart_data);
  for (int i = 0; i < strlen(uart_data); i++) {
    if (uart_data[i] == c) {
      count++;
    }
  }
  if (count == 14) {
    return true;
  } else {
    return false;
  }
}

static void gps__parse_coordinates_from_line(void) {
  char gps_line[120];
  char *ptr_to_gps_data;
  const char token = ',';
  if (line_buffer__remove_line(&line, gps_line, sizeof(gps_line))) {
    if (gps__check_if__complete_GPGGA_string(gps_line)) {
      ptr_to_gps_data = strtok(gps_line, ",");
      if ((strcmp(ptr_to_gps_data, "$GPGGA") == 0)) {
        // gps_lock_status = false;
        for (int i = 0; i < 9; i++) {
          ptr_to_gps_data = strtok(NULL, ",");
          if (i == 1 && ptr_to_gps_data != NULL) {
            sscanf(ptr_to_gps_data, "%f", &parsed_coordinates.latitude);
            gps_convert_string_to_latitude();
          } else if (i == 2 && ptr_to_gps_data != NULL) {
            if (strcmp("S", ptr_to_gps_data) == 0) {
              parsed_coordinates.latitude *= -1;
            }
          } else if (i == 3 && ptr_to_gps_data != NULL) {
            sscanf(ptr_to_gps_data, "%f", &parsed_coordinates.longitude);
            gps_convert_string_to_longitude();
            // gps_lock_status = true;
          } else if (i == 4 && ptr_to_gps_data != NULL) {
            if (strcmp("W", ptr_to_gps_data) == 0) {
              parsed_coordinates.longitude *= -1;
            }
          } else {
            // do nothing
          }
        }
      } else {
        // do nothing;
      }
    }
  }
}

/***********************************************************************************************************************
 *
 *                                          P U B L I C   F U N C T I O N S
 *
 **********************************************************************************************************************/

void gps__init(void) {
  line_buffer__init(&line, line_buffer, sizeof(line_buffer));
  gpio__construct_with_function(GPIO__PORT_2, 8, GPIO__FUNCTION_2);
  gpio__construct_with_function(GPIO__PORT_2, 9, GPIO__FUNCTION_2);
  gpio__construct_with_function(GPIO__PORT_0, 6, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_input(gps_fix);

  uart__init(gps_uart, clock__get_peripheral_clock_hz(), 9600);

  QueueHandle_t rxq_handle = xQueueCreate(2048, sizeof(char));
  QueueHandle_t txq_handle = xQueueCreate(512, sizeof(char));
  uart__enable_queues(gps_uart, rxq_handle, txq_handle);

  parsed_coordinates.latitude = 0;
  parsed_coordinates.longitude = 0;
  delay__ms(1000);
}

void gps__setup_command_registers(void) {
  if (!baud_changed) {
    static const char baud_rate[] = "$PMTK251,38400*27\r\n";
    gps__nema(baud_rate, sizeof(baud_rate));
    delay__ms(500);
    lpc_peripheral__turn_off_power_to(LPC_PERIPHERAL__UART2);
    uart__init(gps_uart, clock__get_peripheral_clock_hz(), 38400);
    baud_changed = true;
  }

  if ((is_setup == false) && baud_changed) {
    static const char baud_ack[] = "$PMTK001,251,3*36\r\n";
    char buf[128];
    static const char update_rate_10hz[] = "$PMTK220,100*2F\r\n";
    static const char disable_except_gpgga[] = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
    static const char query_dgps[] = "$PMTK401*37\r\n";
    static const char set_dgps[] = "$PMTK301,2*2E\r\n ";
    static const char set_sbas[] = "$PMTK313,1*2E\r\n";

    gps__nema(disable_except_gpgga, sizeof(disable_except_gpgga));
    gps__nema(update_rate_10hz, sizeof(update_rate_10hz));
    gps__nema(set_dgps, sizeof(set_dgps));
    gps__nema(set_sbas, sizeof(set_sbas));
    is_setup = true;
  }
}
void compass__get_current_heading_fake(void) {
  static float heading = 40;
  if (gpio__get(board_io__get_sw0())) {
    heading += 5;
  } else if (gpio__get(board_io__get_sw1())) {
    heading -= 5;
  } else if (gpio__get(board_io__get_sw2())) {
    heading += 1;
  } else if (gpio__get(board_io__get_sw3())) {
    heading -= 1;
  } else {
    // do nothing
  }
  parsed_coordinates.heading = heading;
}

bool gps_is_fixed(void) {
  const bool gps_fixed = false; // active low so gps gets data only if false
  if (gpio__get(gps_fix) == gps_fixed) {
    return true;
  } else {
    return false;
  }
}

void gps__run_once(void) {
  gpio__set(board_io__get_led3());
  if (gps_is_fixed() && is_setup) {
    gpio__reset(board_io__get_led3());
    gps__transfer_data_from_uart_driver_to_line_buffer();
    gps__parse_coordinates_from_line();
    print__gps_coordinates();
  }
}

gps_coordinates_t gps__get_coordinates(void) { return parsed_coordinates; }

void print__gps_coordinates(void) {
  printf("Longitude: %f \t Latitude: %f\n", parsed_coordinates.longitude, parsed_coordinates.latitude);
}

// debug functions
