#include "MotorSpeedStatus.h"
#include "can_bus.h"
#include "can_module.h"
#include "can_packet.h"
#include "delay.h"
#include <stdbool.h>
#include <stdio.h>
uint8_t motorState = 0;

static bool isMotorCalibrated = false;

void MotorSpeedStatus_setMotorCurrentState(motor_type_t motorAxis, uint8_t stateValue) { motorState = stateValue; }

uint8_t MotorSpeedStatus_getMotorCurrentState(motor_type_t motorAxis) { return motorState; };

void motorCalibrationSequence(void) {
  static uint8_t state = 0;
  static uint8_t enterCalibrating = false;
  printf("STATE: %i\n", state);
  switch (state) {

  case 0:
    if (isMotorCalibrated == false && motorState == 1) {
      dbc_Set_Axis_State_s axisState;
      axisState.Axis_Requested_State = 0x03;
      can__msg_t can_msg = {};
      const dbc_message_header_t canMsgData = dbc_encode_Set_Axis_State(can_msg.data.bytes, &axisState);
      can_msg.msg_id = canMsgData.message_id | (0x3 << 5);
      can_msg.frame_fields.data_len = canMsgData.message_dlc;
      can__tx(can1, &can_msg, 0);
      state = 1;
    }
    break; /* optional */

  case 1:
    if (motorState == 4 || motorState == 7) {
      state = 2;
      enterCalibrating = true;
    }
    printf("STATE 1\n");
    break;
  case 2:
    if (motorState == 1 && enterCalibrating == true) {
      state = 3;
    }

    break; /* optional */

  case 3:
    isMotorCalibrated = true;

  /* you can have any number of case statements */
  default: /* Optional */
    // do nothing
    break;
  }
}

bool getMotorCalibrationStatus(void) { return isMotorCalibrated; }