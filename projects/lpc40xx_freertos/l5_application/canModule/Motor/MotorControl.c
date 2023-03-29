#include "MotorControl.h"
#include "MotorSpeedStatus.h"
#include "can_bus.h"
#include "can_module.h"
#include "can_packet.h"
#include "delay.h"
#include "project.h"

#define MOTOR_CAN_ID_BIT_POSITION 5

static motor_parameters_e motorControl[NUM_OF_MOTORS] = {
    {.motorAxisType = MOTOR_0,
     .inputSpeed = 0,
     .inputTorque = 0.05513333333f,
     .isCalibrated = false,
     .calibrationState = 0x0,
     .axisCanID = 0x03,
     .motorState_e = 0},
    {.motorAxisType = MOTOR_1,
     .inputSpeed = 0,
     .inputTorque = 0.05513333333f,
     .isCalibrated = false,
     .calibrationState = 0x0,
     .axisCanID = 0x01,
     .motorState_e = 0},
};

void MotorControl_sendVelocityData(motor_axis_e motorSide) {
  dbcSetInputVels velocityData;
  velocityData.Input_Torque_FF = (float)motorControl[motorSide].inputTorque;
  velocityData.Input_Vel = (float)motorControl[motorSide].inputSpeed;
  // send current destination information
  can__msg_t can_msg = {};
  const dbc_message_header_t canMsgData = dbcEncodeVelocityData(can_msg.data.bytes, &velocityData);
  printf("canMsgData.message_id: %i", canMsgData.message_id);
  can_msg.msg_id = canMsgData.message_id | (motorControl[motorSide].axisCanID << MOTOR_CAN_ID_BIT_POSITION);
  can_msg.frame_fields.data_len = canMsgData.message_dlc;
  can__tx(can1, &can_msg, 0);
}

void MotorControl_updateVelocityData(float inputVelocity, motor_axis_e motorSide) {
  motorControl[motorSide].inputSpeed = inputVelocity;
}

void MotorControl_calibrateMotors(motor_axis_e motorSide) {
  static uint8_t state = 0;
  static uint8_t enterCalibrating = false;
  printf("STATE: %i\n", state);
  switch (state) {

  case 0:
    if (motorControl[motorSide].isCalibrated == false && motorControl[motorSide].motorState_e == IDLE) {
      dbc_Set_Axis_State_s axisState;
      can__msg_t can_msg = {};
      const dbc_message_header_t canMsgData = dbc_encode_Set_Axis_State(can_msg.data.bytes, &axisState);
      can_msg.msg_id = canMsgData.message_id | (motorControl[motorSide].axisCanID << MOTOR_CAN_ID_BIT_POSITION);
      can_msg.frame_fields.data_len = canMsgData.message_dlc;
      can__tx(can1, &can_msg, 0);
      state = 1;
    }
    break; /* optional */

  case 1:
    if (motorControl[motorSide].motorState_e == MOTOR_CALIBRATION ||
        motorControl[motorSide].motorState_e == ENCODER_OFFSET_CALIBRATION) {
      state = 2;
      enterCalibrating = true;
    }
    printf("STATE 1\n");
    break;
  case 2:
    if (motorControl[motorSide].motorState_e == IDLE && enterCalibrating == true) {
      state = 3;
    }

    break; /* optional */

  case 3:
    motorControl[motorSide].isCalibrated = true;

  /* you can have any number of case statements */
  default: /* Optional */
    // do nothing
    break;
  }
}