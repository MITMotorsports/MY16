#include "Motor_Handler.h"
#include "Can_Controller.h"
#include "Led_Controller.h"

const int MOTOR_CURRENT_MODIFIER = 32; //0x20
const int MOTOR_SPEED_MODIFIER = 48; //0x30
const int MOTOR_TEMP_MODIFIER = 73; //0x49
const int OUTPUT_TEMP_MODIFIER = 74; //0x4A
const int AIR_TEMP_MODIFIER = 75; //0x4B
const int MOTOR_POSITION_MODIFIER = 109; //0x6D
const int MOTOR_ERRORS_MODIFIER = 143; //0x8F
const int MOTOR_STATUS_REQUEST_MODIFIER = 61; //0x3d

void Motor_Handler::begin() {
  // No initialization needed
}

void Motor_Handler::requestAllUpdates() {
  requestSingleUpdate(MOTOR_CURRENT_MODIFIER);
  requestSingleUpdate(MOTOR_SPEED_MODIFIER);
  requestSingleUpdate(MOTOR_TEMP_MODIFIER);
  requestSingleUpdate(MOTOR_POSITION_MODIFIER);
  requestSingleUpdate(MOTOR_ERRORS_MODIFIER);
}

void Motor_Handler::requestSingleUpdate(unsigned char id) {
  Frame positiveFrame = {.id=POSITIVE_MOTOR_REQUEST_ID, .body={MOTOR_STATUS_REQUEST_MODIFIER, id}};
  Frame negativeFrame = {.id=NEGATIVE_MOTOR_REQUEST_ID, .body={MOTOR_STATUS_REQUEST_MODIFIER, id}};
  CAN().write(positiveFrame);
  CAN().write(negativeFrame);
}

void Motor_Handler::handleMessage(Frame& message) {
  int sign = 0;
  if(message.id == POSITIVE_MOTOR_ID) {
    sign = 1;
  }
  else if(message.id == NEGATIVE_MOTOR_ID) {
    sign = -1;
  }
  else {
    return;
  }
  switch(message.body[0]) {
    case MOTOR_SPEED_MODIFIER:
      processSpeedMessage(message, sign);
  }
}

void Motor_Handler::processSpeedMessage(Frame& message, int sign) {
  unsigned char hi_order = message.body[2];
  unsigned char low_order = message.body[1];
  uint16_t concat = (hi_order << 8) + low_order;
  int16_t speed = (int16_t) concat;

  // Scale speed from [0:32767 (aka 2^15 - 1)] to [0:30]
  // This magic number is just 32767/30 rounded
  int scaling_factor = 1092;
  unsigned char scaled_speed = (speed * sign) / scaling_factor;
  LED().set_lightbar_power(scaled_speed);
}
