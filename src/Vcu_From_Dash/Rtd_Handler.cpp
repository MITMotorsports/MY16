#include <Debouncer.h>
#include <PciManager.h>

#include "Rtd_Handler.h"
#include "Rtd_Controller.h"
#include "Dispatch_Controller.h"

const int RTD_BUTTON = 6;

// Non-member variable used in timer function pointers
bool enableFired = false;

void enableAll() {
  enableFired = true;
  RTD().enable();
  Dispatcher().enable();
  Frame enableMessage = { .id=DASH_ID, .body={1}};
  CAN().write(enableMessage);
  Serial.println(F("Rtd_Handler Enabled"));
}

void disableAll() {
  enableFired = false;
  RTD().disable();
  Dispatcher().enable();
  Frame disableMessage = { .id=DASH_ID, .body={0}};
  CAN().write(disableMessage);
  Serial.println(F("Rtd_Handler Disabled"));
}

bool enable(Task*) {
  enableAll();
  return false;
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
    disableAll();
  }
}

// Must be declared at global scope in order to preserve reference
Debouncer debouncer(RTD_BUTTON, MODE_OPEN_ON_PUSH, RTDPressed, RTDReleased);

void Rtd_Handler::begin() {
  pinMode(RTD_BUTTON, INPUT);
  PciManager.registerListener(RTD_BUTTON, &debouncer);
  RTD().disable();
  Serial.println(F("Rtd_Handler Begun"));
}

void Rtd_Handler::handleMessage(Frame& frame) {
  if(frame.body[0]) {
    Serial.println(F("Received start message"));
    enableAll();
  }
  else {
    Serial.println(F("Received stop message"));
    disableAll();
  }
}


