#include "Dispatch_Controller.h"
#include "Motor_Handler.h"

#include "Can_Controller.h"
#include "Rtd_Controller.h"

const int REQUEST_PREFIX = 61; //0x3D
const int TORQUE_PREFIX = 144; //0x90

const int MOTOR_CURRENT_MODIFIER = 32; //0x20
const int MOTOR_SPEED_MODIFIER = 48; //0x30
const int MOTOR_TEMP_MODIFIER = 73; //0x49
const int OUTPUT_TEMP_MODIFIER = 74; //0x4A
const int AIR_TEMP_MODIFIER = 75; //0x4B
const int MOTOR_POSITION_MODIFIER = 109; //0x6D
const int MOTOR_ERRORS_MODIFIER = 143; //0x8F

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
  Frame positiveFrame = {.id=POSITIVE_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, id, 0}, .len=3};
  Frame negativeFrame = {.id=NEGATIVE_MOTOR_REQUEST_ID, .body={REQUEST_PREFIX, id, 0}, .len=3};
  CAN().write(positiveFrame);
  CAN().write(negativeFrame);
}

void Motor_Handler::handleMessage(Frame& message) {
  // Enable-only task
  if(!RTD().isEnabled()) {
    return;
  }

  int sign;
  if(message.id == POSITIVE_MOTOR_ID) {
    sign = 1;
  }
  else if(message.id == NEGATIVE_MOTOR_ID) {
    // TODO make negative if necessary
    sign = 1;
  }
  else {
    return;
  }
  switch(message.body[0]) {
    case MOTOR_SPEED_MODIFIER:
      // TODO eventually log this
      break;
  }
}
