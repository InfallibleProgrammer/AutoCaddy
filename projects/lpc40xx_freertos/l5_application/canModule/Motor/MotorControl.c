#include "MotorControl.h"
#include "SoftwareTimer.h"
#include "can_bus.h"
#include "can_module.h"
#include "can_packet.h"
#include "delay.h"
#include "project.h"

#define MOTOR_CAN_ID_BIT_POSITION 5
#define MAX_CALIBRATION_TIME_MS 3000

static software_timer_s calibrationTime;

static motor_parameters_e motorControl[NUM_OF_MOTORS] = {
    {.motorAxisType = MOTOR_0,
     .inputSpeed = 0,
     .inputTorque = 0.05513333333f,
     .isCalibrated = false,
     .calibrationState = INITIAL_CALIBRATION_STATE,
     .axisCanID = 0x3,
     .motorState_e = 0},
    {.motorAxisType = MOTOR_1,
     .inputSpeed = 0,
     .inputTorque = 0.05513333333f,
     .isCalibrated = false,
     .calibrationState = INITIAL_CALIBRATION_STATE,
     .axisCanID = 0x1,
     .motorState_e = 0},
};

static bool motorSystemCalibrated = false;

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
  if (motorSystemCalibrated == true) {
    motorControl[motorSide].inputSpeed = inputVelocity;
  }
}

void MotorControl_setState(motor_axis_e motorSide, axis_state_e stateValue) {
  motorControl[motorSide].motorState_e = stateValue;
}

void MotorControl_calibrateMotors(motor_axis_e motorSide) {
  bool timeMet;
  switch (motorControl[motorSide].calibrationState) {

  case INITIAL_CALIBRATION_STATE:
    if (motorControl[motorSide].isCalibrated == false && motorControl[motorSide].motorState_e == IDLE) {
      dbc_Set_Axis_State_s axisState;
      axisState.Axis_Requested_State = ENCODER_INDEX_SEARCH;
      can__msg_t can_msg = {};
      const dbc_message_header_t canMsgData = dbc_encode_Set_Axis_State(can_msg.data.bytes, &axisState);
      can_msg.msg_id = canMsgData.message_id | (motorControl[motorSide].axisCanID << MOTOR_CAN_ID_BIT_POSITION);
      can_msg.frame_fields.data_len = canMsgData.message_dlc;
      can__tx(can1, &can_msg, 0);
      SoftwareTimer_startTime(&calibrationTime, MAX_CALIBRATION_TIME_MS);
      motorControl[motorSide].calibrationState = ENCODER_CALIBRATION_STAGE;
    }
    break; /* optional */

  case ENCODER_CALIBRATION_STAGE:
    timeMet = SoftwareTimer_hasTimeExpired(&calibrationTime);
    if ((motorControl[motorSide].motorState_e == ENCODER_INDEX_SEARCH) || (timeMet == true)) {
      motorControl[motorSide].calibrationState = ENCODER_CALIBRATED
    }
    break; /* optional */

  case ENCODER_CALIBRATED:
    motorControl[motorSide].isCalibrated = true;
    break;
  /* you can have any number of case statements */
  default: /* Optional */
    // do nothing
    break;
  }
}

void MotorControl_motorCalibrationSequence(void) {
  if (motorControl[MOTOR_0].isCalibrated == false) {
    MotorControl_calibrateMotors(MOTOR_0);
  } else if (motorControl[MOTOR_1].isCalibrated == false) {
    MotorControl_calibrateMotors(MOTOR_1);
  } else {
    motorSystemCalibrated = true;
  }
}
uint8_t MotorControl_getMotorCANID(motor_axis_e motorSide) { return motorControl[motorSide].axisCanID; }
bool MotorControl_isMotorCalibrated(motor_axis_e motorSide) { return motorControl[motorSide].isCalibrated; }
void MotorControl_init(void) { SoftwareTimer_init(&calibrationTime); }