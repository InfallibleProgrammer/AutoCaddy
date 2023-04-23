#include "can_packet.h"

dbc_message_header_t dbcEncodeVelocityData(uint8_t bytes[8], const dbcSetInputVels *message) {
  memset(bytes, 0, 8);
  memcpy(&bytes[0], &(message->Input_Vel), sizeof(uint32_t));
  memcpy(&bytes[4], &(message->Input_Torque_FF), sizeof(uint32_t));
  return dbc_header_Set_Input_Vel;
}

bool dbcDecodeHeartbeat(dbc_Heartbeat_s *message, dbc_message_header_t header, uint8_t axis_ID,
                        const uint8_t bytes[8]) {
  const bool success = true;
  if ((header.message_id != (dbc_header_Heartbeat.message_id + (axis_ID << 5))) ||
      (header.message_dlc != dbc_header_Heartbeat.message_dlc)) {
    return !success;
  }

  uint64_t raw = 0;
  raw = ((bytes[0]));                  // extract 8 bit(s) to bit position 0
  raw |= (uint64_t)((bytes[1])) << 8;  // extract 8 bit(s) to bit position 8
  raw |= (uint64_t)((bytes[2])) << 16; // extract 8 bit(s) to bit position 16
  raw |= (uint64_t)((bytes[3])) << 24; // extract 8 bit(s) to bit position 24
  message->Axis_Error = raw;

  raw = ((bytes[4])); // extract 8 bit(s) to bit position 0
  message->Axis_State = raw;

  raw = ((bytes[5])); // extract 8 bit(s) to bit position 0
  message->Motor_Flags = raw;

  raw = ((bytes[6])); // extract 8 bit(s) to bit position 0
  message->Encoder_Flags = raw;

  raw = ((bytes[7])); // extract 8 bit(s) to bit position 0
  message->Controller_Flags = raw;

  message->mia_info.mia_counter = 0;
  return success;
}