#include "can_module.h"
#include "MotorControl.h"
#include "MotorLogic.h"
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
  dbc_COMPASS_HEADING_DISTANCE_s gpsData = {};
  uint8_t motorAxisID;
  // Receive all messages
  while (can__rx(can1, &can_msg, 5)) {
    const dbc_message_header_t header = {
        .message_id = can_msg.msg_id,
        .message_dlc = can_msg.frame_fields.data_len,
    };
    // Message ID of AXIS0
    motorAxisID = MotorControl_getMotorCANID(MOTOR_0);
    if (can_msg.msg_id & (motorAxisID << 5)) {
      if (dbcDecodeHeartbeat(&heartBeatData, header, motorAxisID, can_msg.data.bytes)) {
        MotorControl_setState(MOTOR_0, heartBeatData.Axis_State);
      }
    }
    // Message ID of AXIS1
    if (can_msg.msg_id & (motorAxisID << 5)) {
      motorAxisID = MotorControl_getMotorCANID(MOTOR_1);
      if (dbcDecodeHeartbeat(&heartBeatData, header, motorAxisID, can_msg.data.bytes)) {
        MotorControl_setState(MOTOR_1, heartBeatData.Axis_State);
      }
    }
    if (dbc_decode_COMPASS_HEADING_DISTANCE(&gpsData, header, can_msg.data.bytes)) {
      updateMotorValues(gpsData.DESTINATION_HEADING, gpsData.DISTANCE);
    }
  }
}

void periodic_callbacks_1Hz_Velocity(void) {
  MotorControl_sendVelocityData(MOTOR_0);
  MotorControl_sendVelocityData(MOTOR_1);
}

void periodic_callbacks_1Hz_sendspeed(void) {
  can__msg_t can_msg = {};
  dbc_Motor_SpeedLog_s motorSpeed;
  motorSpeed.MotorSpeed0_rpm = MotorControl_getMotorSpeed(MOTOR_0);
  motorSpeed.MotorSpeed1_rpm = MotorControl_getMotorSpeed(MOTOR_1);
  printf("motorSpeed: %f %f", motorSpeed.MotorSpeed0_rpm, motorSpeed.MotorSpeed1_rpm);
  const dbc_message_header_t canMsgData = dbc_encode_Motor_SpeedLog(can_msg.data.bytes, &motorSpeed);
  can_msg.msg_id = canMsgData.message_id;
  can_msg.frame_fields.data_len = canMsgData.message_dlc;
  can__tx(can1, &can_msg, 0);
}
