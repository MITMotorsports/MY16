#include "Dispatch_Controller.h"
#include "Motor_Handler.h"

#include "Can_Controller.h"
#include "Rtd_Controller.h"
#include "Store_Controller.h"

const int REQUEST_PREFIX = 61; //0x3D
const int TORQUE_PREFIX = 144; //0x90

const float MOTOR_RPM_SCALING_FACTOR = 8.2;

const int MOTOR_CURRENT_MODIFIER = 32; //0x20
const int MOTOR_SPEED_MODIFIER = 48; //0x30
const int MOTOR_TEMP_MODIFIER = 73; //0x49
const int OUTPUT_TEMP_MODIFIER = 74; //0x4A
const int AIR_TEMP_MODIFIER = 75; //0x4B
const int MOTOR_POSITION_MODIFIER = 109; //0x6D
const int MOTOR_ERRORS_MODIFIER = 143; //0x8F
const int MOTOR_VOLTAGE_MODIFIER = 224; //0xE0

void Motor_Handler::begin() {
  // No initialization needed
}

void Motor_Handler::requestSingleVoltageUpdate() {
  Frame frame = {.id=LEFT_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, MOTOR_VOLTAGE_MODIFIER, 0}, .len=3};
  CAN().write(frame);

  frame = {.id=RIGHT_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, MOTOR_VOLTAGE_MODIFIER, 0}, .len=3};
  CAN().write(frame);
}

void Motor_Handler::requestPermanentVoltageUpdate() {
  Frame frame = {.id=LEFT_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, MOTOR_VOLTAGE_MODIFIER, 100}, .len=3};
  CAN().write(frame);

  frame = {.id=RIGHT_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, MOTOR_VOLTAGE_MODIFIER, 100}, .len=3};
  CAN().write(frame);
}

void Motor_Handler::requestPermanentUpdates(uint16_t can_id) {
  requestPermanentUpdate(can_id, MOTOR_CURRENT_MODIFIER, 101);
  requestPermanentUpdate(can_id, MOTOR_SPEED_MODIFIER, 103);
  requestPermanentUpdate(can_id, MOTOR_TEMP_MODIFIER, 105);
  requestPermanentUpdate(can_id, MOTOR_POSITION_MODIFIER, 107);
  requestPermanentUpdate(can_id, MOTOR_ERRORS_MODIFIER, 109);
  requestPermanentUpdate(can_id, MOTOR_VOLTAGE_MODIFIER, 113);
}

void Motor_Handler::requestPermanentUpdate(uint16_t can_id, uint8_t msg_type, uint8_t time) {
  Frame frame = {.id=can_id, .body={REQUEST_PREFIX, msg_type, time}, .len=3};
  CAN().write(frame);
}

int16_t mergeBytesOfSignedInt(uint8_t low_byte, uint8_t high_byte) {
  int16_t result = (high_byte << 8) + low_byte;
  return result;
}

void Motor_Handler::handleMessage(Frame& message) {
  // Enable-only task
  if(!(message.id == RIGHT_MOTOR_ID || message.id == LEFT_MOTOR_ID)) {
    return;
  }
  if(message.id == RIGHT_MOTOR_ID) {
    Store().logMotorResponse(Store().RightMotor);
  }
  else {
    Store().logMotorResponse(Store().LeftMotor);
  }

  // Serial.print("motor_id: ");
  // Serial.print(message.id);
  // Serial.print(", msg_id: ");
  // Serial.print(message.body[0]);
  switch(message.body[0]) {
    case MOTOR_SPEED_MODIFIER:
      // TODO eventually log this
      // Serial.print(", speed_raw: ");
      // int signed_speed = mergeBytesOfSignedInt(message.body[1], message.body[2]);
      // Serial.print(signed_speed);
      // int converted_speed = signed_speed / MOTOR_RPM_SCALING_FACTOR;
      // Serial.print(", speed_rpm: ");
      // Serial.print(converted_speed);
      break;
  }
  // Serial.println("");
}
