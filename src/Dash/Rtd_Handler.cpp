#include "Rtd_Handler.h"

// Non-member variable used in timer function pointers
bool enableFired = false;

Debouncer debouncer(RTD_BUTTON_PIN, MODE_CLOSE_ON_PUSH, pressRtdButton, releaseRtdButton);

void Rtd_Handler::begin() {
  pinMode(RTD_BUTTON_PIN, INPUT);
  PciManager.registerListener(RTD_BUTTON_PIN, &debouncer);
}

void sendEnableRequest() {
  Frame enableMessage = { .id=DASH_ID, .body={1}};
  CAN().write(enableMessage);
}

void sendDisableRequest() {
  Frame disableMessage = { .id=DASH_ID, .body={0}};
  CAN().write(disableMessage);
}

bool sendEnableRequestWrapper(Task*) {
  sendEnableRequest();
  enableFired = true;
  return false;
}

DelayRun sendEnableRequestTask(500, sendEnableRequestWrapper);

void pressRtdButton() {
  // The enable task will fire automatically if held for >1000ms
  enableFired = false;
  sendEnableRequestTask.startDelayed();
}

void releaseRtdButton(unsigned long) {
  if(enableFired) {
    // Do nothing since car already enabled before release
    return;
  }
  else {
    // Button released before 500ms, so driver must want to disable
    SoftTimer.remove(&sendEnableRequestTask);
    sendDisableRequest();
  }
}

void Rtd_Handler::handleMessage(Frame& frame) {
  switch(frame.id) {
    case VCU_ID:
      processVcuMessage(frame);
      break;
    case BMS_SOC_ID:
      processSocMessage(frame);
      break;
    case POSITIVE_MOTOR_ID:
    case NEGATIVE_MOTOR_ID:
      processSpeedMessage(frame);
      break;
  }
}

void Rtd_Handler::processVcuMessage(Frame& message) {
  if(message.body[0]) {
    RTD().enable();
  }
  else {
    RTD().disable();
  }
}

void Rtd_Handler::processSpeedMessage(Frame& message) {
  unsigned char hi_order = message.body[2];
  unsigned char low_order = message.body[1];
  uint16_t concat = (hi_order << 8) + low_order;
  int16_t speed = (int16_t) concat;

  // Scale speed from [0:32767 (aka 2^15 - 1)] to [0:30]
  // This magic number is just 32767/30 rounded
  int scaling_factor = 1092;
  unsigned char scaled_speed = speed / scaling_factor;
  LED().set_lightbar_power(scaled_speed);
}

void Rtd_Handler::processSocMessage(Frame& frame) {
  unsigned char SOC = frame.body[0];
  // Scale SOC from [0:100] to [0:30]
  double scaling_factor = 3.33333;
  SOC = SOC / scaling_factor;
  LED().set_lightbar_battery(SOC);
}
