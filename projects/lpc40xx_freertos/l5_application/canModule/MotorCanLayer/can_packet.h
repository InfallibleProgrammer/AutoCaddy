#include "MotorSpeedStatus.h"
#include "project.h"
#include <stdint.h>
typedef struct {
  dbc_mia_info_t mia_info;

  float Input_Torque_FF;
  float Input_Vel;
} dbcSetInputVels;

dbc_message_header_t dbcEncodeVelocityData(uint8_t bytes[8], const dbcSetInputVels *message);
bool dbcDecodeHeartbeat(dbc_Heartbeat_s *message, dbc_message_header_t header, uint8_t axis_ID, const uint8_t bytes[8]);

void initCanMotorPackets(uint32_t axis0_ID, uint32_t axis1_ID);