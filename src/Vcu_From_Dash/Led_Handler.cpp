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
  else if (message.id == BMS_SOC_ID) {
    processBmsMessage(message.body);
  }
}

void Led_Handler::processSpeedMessage(unsigned char message[8], int sign) {
  switch(message[0]) {
    // Speed reference
    case 48:
      unsigned char hi_order = message[2];
      unsigned char low_order = message[1];
      unsigned short concat = (hi_order << 8) + low_order;
      int16_t speed = *((int16_t*)(&concat));

      // Scale speed from [0:32767 (aka 2^15 - 1)] to [0:30]
      // This magic number is just 32767/30 rounded
      int scaling_factor = 1092;
      unsigned char scaled_speed = (speed * sign) / scaling_factor;
      LED().set_lightbar_power(scaled_speed);
  }
}

void Led_Handler::processBmsMessage(unsigned char message[8]) {
  unsigned char SOC = message[0];
  // Scale SOC from [0:100] to [0:30]
  double scaling_factor = 3.33333;
  SOC = SOC / scaling_factor;
  LED().set_lightbar_battery(SOC);
}

