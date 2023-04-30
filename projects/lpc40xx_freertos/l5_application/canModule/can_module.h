#pragma once

#include "can_bus.h"
#include "project.h"

// Invoke this method in your periodic callbacks
void can_bus_handler__process_all_received_messages(void);
void periodic_callbacks_1Hz_Velocity(void);
void periodic_callbacks_1Hz_sendspeed(void);
// // debug functions
// dbc_DBG_GEO_CAN_STATUS_s can_bus_status(void);
