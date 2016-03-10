#include <Debouncer.h>
#include <PciManager.h>

#include "Rtd_Handler.h"
#include "Rtd_Controller.h"

// Non-member variable used in timer function pointers
bool enableFired = false;

bool enable(Task*) {
  enableFired = true;
  // Tell dash to enable
  RTD().enable();
  // Tell VCU to enable
  CAN().write(ENABLE_REQUEST);
  return false;
  Serial.println(F("Rtd_Handler Enabled"));
}

DelayRun enableTask(500, enable);

void RTDPressed() {
  // The enable task will fire automatically if held for >1000ms
  enableFired = false;
  enableTask.startDelayed();
}

void RTDReleased(unsigned long) {
  if(enableFired) {
    // Do nothing since car already enabled before release
    return;
  }
  else {
    // Button released before 1000ms, so driver must want to disable
    SoftTimer.remove(&enableTask);
    // Tell dash to disable
    RTD().disable();
    // Tell VCU to disable
    CAN().write(DISABLE_REQUEST);
    Serial.println(F("Rtd_Handler Disabled"));
  }
}

// Must be declared at global scope in order to preserve reference
Debouncer debouncer(RTD_BUTTON, MODE_OPEN_ON_PUSH, RTDPressed, RTDReleased);

void Rtd_Handler::begin() {
  pinMode(RTD_BUTTON, INPUT);
  PciManager.registerListener(RTD_BUTTON, &debouncer);
  Serial.println(F("Rtd_Handler Begin"));
}

void Rtd_Handler::handleMessage(Frame& frame) {
  if(frame.message[0]) {
    Serial.println(F("Received start message"));
    RTD().enable();
  }
  else {
    Serial.println(F("Received stop message"));
    RTD().disable();
  }
}


