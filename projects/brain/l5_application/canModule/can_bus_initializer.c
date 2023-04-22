#include "can_bus_initializer.h"
#include "can_bus.h"
#include <stddef.h>
#include <stdio.h>

bool can_bus_initializer(void) {
  bool was_can_initialization_successful = false;
  const can__num_e CAN_PORT = can1;
  const uint32_t baud_rate_in_kbps = 200;
  const uint16_t rx_queue_size = 200;
  const uint16_t tx_queue_size = 100;

  // @param bus_off_cb  The callback function when CAN BUS enters BUS error state - optional
  // @param data_ovr_cb The callback function when CAN BUS encounters data-overrun - optional
  printf("BEFORE HERE\n");
  if (can__init(CAN_PORT, baud_rate_in_kbps, rx_queue_size, tx_queue_size, NULL, NULL)) {
    printf("IN HERE\n");
    can__bypass_filter_accept_all_msgs();
    can__reset_bus(CAN_PORT);
    was_can_initialization_successful = true;
  } else {
    // do nothing
  }
  return was_can_initialization_successful;
}
