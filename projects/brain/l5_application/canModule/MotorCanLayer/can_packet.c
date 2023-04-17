#include "can_packet.h"
static uint32_t axisID[NUM_OF_AXIS] = {0, 1};

dbc_message_header_t dbcEncodeVelocityData(uint8_t bytes[8], const dbcSetInputVels *message) {
  memset(bytes, 0, 8);
  memcpy(&bytes[0], &(message->Input_Vel), sizeof(uint32_t));
  memcpy(&bytes[4], &(message->Input_Torque_FF), sizeof(uint32_t));
  return dbc_header_Set_Input_Vel;
}

void initCanMotorPackets(uint32_t axis0_ID, uint32_t axis1_ID) {
  axisID[AXIS_0] = axis0_ID;
  axisID[AXIS_1] = axis1_ID;
}