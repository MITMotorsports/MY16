#include "Led_Handler.h"
#include "Led_Controller.h"

void Led_Handler::begin() {
  // No initialization needed
}

void Led_Handler::handleMessage(Frame& message) {
  if (message.id == POSITIVE_MOTOR_ID) {
    processSpeedMessage(message.body, 1);
  }
  else if (message.id == NEGATIVE_MOTOR_ID) {
    processSpeedMessage(message.body, -1);
  }
}

void Led_Handler::processSpeedMessage(unsigned char message[8], int sign) {
}
