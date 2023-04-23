#include "can_module.h"
#include "MotorControl.h"
#include "MotorSpeedStatus.h"
#include "can_bus.h"
#include "can_packet.h"
#include "delay.h"
#include "project.h"

static void priv_sendVelocityData(void) {
  dbcSetInputVels velocityData;
  velocityData.Input_Torque_FF = (float)0.05513333333f;
  velocityData.Input_Vel = (float)5;
  // send current destination information
  can__msg_t can_msg = {};
  const dbc_message_header_t canMsgData = dbcEncodeVelocityData(can_msg.data.bytes, &velocityData);
  printf("canMsgData.message_id: %i", canMsgData.message_id);
  can_msg.msg_id = canMsgData.message_id | (0x3 << 5);
  can_msg.frame_fields.data_len = canMsgData.message_dlc;
  can__tx(can1, &can_msg, 0);
}

static void priv_sendCalibrationData(void) {
  dbc_Set_Axis_State_s axisState;
  axisState.Axis_Requested_State = 0x03;
  can__msg_t can_msg = {};
  const dbc_message_header_t canMsgData = dbc_encode_Set_Axis_State(can_msg.data.bytes, &axisState);
  can_msg.msg_id = canMsgData.message_id | (0x3 << 5);
  can_msg.frame_fields.data_len = canMsgData.message_dlc;
  can__tx(can1, &can_msg, 0);
}

static void priv_enableCloseLoopControl(void) {
  dbc_Set_Axis_State_s axisState;
  axisState.Axis_Requested_State = 0x08;
  can__msg_t can_msg = {};
  const dbc_message_header_t canMsgData = dbc_encode_Set_Axis_State(can_msg.data.bytes, &axisState);
  can_msg.msg_id = canMsgData.message_id | (0x3 << 5);
  can_msg.frame_fields.data_len = canMsgData.message_dlc;
  can__tx(can1, &can_msg, 0);
}

static bool isInitialized = false;
static uint16_t value;
void can_bus_handler__process_all_received_messages(void) {
  can__msg_t can_msg = {0};
  dbc_Heartbeat_s heartBeatData = {};
  uint8_t motorAxisID;
  // Receive all messages
  while (can__rx(can1, &can_msg, 5)) {
    const dbc_message_header_t header = {
        .message_id = can_msg.msg_id,
        .message_dlc = can_msg.frame_fields.data_len,
    };
    // Message ID of AXIS0
    if (can_msg.msg_id & (0x3 << 5)) {
      motorAxisID = MotorControl_getMotorCANID(MOTOR_0);
      if (dbcDecodeHeartbeat(&heartBeatData, header, motorAxisID, can_msg.data.bytes)) {
        MotorControl_setState(MOTOR_AXIS_0, heartBeatData.Axis_State);
      }
    }
    // Message ID of AXIS1
    if (can_msg.msg_id & (0x1 << 5)) {
      motorAxisID = MotorControl_getMotorCANID(MOTOR_1);
      if (dbcDecodeHeartbeat(&heartBeatData, header, 0x1, can_msg.data.bytes)) {
        MotorControl_setState(MOTOR_AXIS_1, heartBeatData.Axis_State);
      }
    }
  }
}

void periodic_callbacks_1Hz_Velocity(void) {

  bool axis0State = MotorSpeedStatus_getMotorCurrentState(MOTOR_AXIS_0);
  static uint8_t counter = 0;
  static bool controlMode = false;
  // if (isInitialized == false) {
  //   priv_sendCalibrationData();
  //   isInitialized = true;
  // }
  // if (isInitialized && controlMode == false) {
  //   priv_enableCloseLoopControl();
  //   controlMode = true;
  // }

  // if (isInitialized && controlMode && axis0State != 4 && axis0State != 7) {
  //   priv_sendVelocityData();
  // }
  counter++;
  if (MotorControl_isMotorCalibrated(AXIS_0) == false && counter >= 5) {
    printf("NOT CALIBRATED\n");
    MotorControl_calibrateMotors(AXIS_0);
  }

  // if (getMotorCalibrationStatus() == false) {
  //   // delay__ms(900);
  //   // motorCalibrationSequence();
  // } else {
  //   if (controlMode == false) {
  //     delay__ms(500);
  //     priv_enableCloseLoopControl();
  //     printf("CONTROL MODE set\n\n\n");
  //     controlMode = true;
  //   }
  //   if (controlMode) {
  //     printf("VELOCITY set\n\n\n");
  //     priv_sendVelocityData();
  //   }
  // }
}

//   void periodic_callbacks_10Hz_GEO_commands(void) {}

// void periodic_callbacks_10Hz_GEO_commands(void) {
//   const dbc_GPS_CURRENT_INFO_s current_RC_location = geo_controller_process_GEO_current_location();
//   const dbc_GPS_CURRENT_DESTINATIONS_DATA_s current_destination_data = determine_destination_location();
//   const dbc_COMPASS_HEADING_DISTANCE_s current_compass_data = determine_compass_heading_and_distance();
//   const dbc_GPS_COMPASS_STATUS_s GPS_compass_status = check_GPS_Compass_Status();
//   //   const dbc_MOTOR_CHANGE_SPEED_AND_ANGLE_MSG_s command = driver__get_motor_commands();

//   // send current destination information
//   can__msg_t can_msg = {};
//   const dbc_message_header_t gps_destination_header =
//       dbc_encode_GPS_CURRENT_DESTINATIONS_DATA(can_msg.data.bytes, &current_destination_data);
//   can_msg.msg_id = gps_destination_header.message_id;
//   can_msg.frame_fields.data_len = gps_destination_header.message_dlc;
//   can__tx(can1, &can_msg, 0);

//   // send heading and distance

//   const dbc_message_header_t heading_and_distance_information =
//       dbc_encode_COMPASS_HEADING_DISTANCE(can_msg.data.bytes, &current_compass_data);
//   can_msg.msg_id = heading_and_distance_information.message_id;
//   can_msg.frame_fields.data_len = heading_and_distance_information.message_dlc;
//   can__tx(can1, &can_msg, 0);

//   const dbc_message_header_t gps_compass_status_header =
//       dbc_encode_GPS_COMPASS_STATUS(can_msg.data.bytes, &GPS_compass_status);
//   can_msg.msg_id = gps_compass_status_header.message_id;
//   can_msg.frame_fields.data_len = gps_compass_status_header.message_dlc;
//   can__tx(can1, &can_msg, 0);

//   // send rc location to bridge
//   const dbc_message_header_t current_RC_location_header =
//       dbc_encode_GPS_CURRENT_INFO(can_msg.data.bytes, &current_RC_location);
//   can_msg.msg_id = current_RC_location_header.message_id;
//   can_msg.frame_fields.data_len = current_RC_location_header.message_dlc;
//   can__tx(can1, &can_msg, 0);
// }

//   // send heading and distance

//   const dbc_message_header_t geo_can_status_header = dbc_encode_DBG_GEO_CAN_STATUS(can_msg.data.bytes,
//   &geo_can_status); can_msg.msg_id = geo_can_status_header.message_id; can_msg.frame_fields.data_len =
//   geo_can_status_header.message_dlc; can__tx(can1, &can_msg, 0);

//   // send rc location to bridge
//   const dbc_message_header_t geo_lock_LED_header =
//       dbc_encode_DBG_GPS_COMPASS_LOCK_LED_CHECK(can_msg.data.bytes, &GEO_LOCK_LED);
//   can_msg.msg_id = geo_lock_LED_header.message_id;
//   can_msg.frame_fields.data_len = geo_lock_LED_header.message_dlc;
//   can__tx(can1, &can_msg, 0);

//   const dbc_message_header_t raw_compass_data_header =
//       dbc_encode_DBG_RAW_COMPASS_DATA(can_msg.data.bytes, &raw_compass_data);
//   can_msg.msg_id = raw_compass_data_header.message_id;
//   can_msg.frame_fields.data_len = raw_compass_data_header.message_dlc;
//   can__tx(can1, &can_msg, 0);
// }
