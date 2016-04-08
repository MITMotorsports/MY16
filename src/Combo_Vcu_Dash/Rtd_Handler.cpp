#include <Debouncer.h>
#include <PciManager.h>

#include "Rtd_Handler.h"
#include "Rtd_Controller.h"
#include "Dispatch_Controller.h"

const int RTD_BUTTON = 6;

// Non-member variable used in timer function pointers
bool enableFired = false;

bool enable(Task*) {
  enableFired = true;
  Dispatcher().enable();
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
    enableFired = false;
    Dispatcher().disable();
  }
}

// Must be declared at global scope in order to preserve reference
Debouncer debouncer(RTD_BUTTON, MODE_OPEN_ON_PUSH, RTDPressed, RTDReleased);

void Rtd_Handler::begin() {
  pinMode(RTD_BUTTON, INPUT);
  PciManager.registerListener(RTD_BUTTON, &debouncer);
  RTD().disable();
}

void Rtd_Handler::handleMessage(Frame& frame) {
  if(frame.id != VCU_ID) {
    return;
  }
  else {
    if(frame.body[0]) {
      Dispatcher().enable();
    }
    else {
      Dispatcher().disable();
    }
  }
}


