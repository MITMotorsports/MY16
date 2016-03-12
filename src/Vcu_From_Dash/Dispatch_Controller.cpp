#include "Dispatch_Controller.h"

//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"
#include "Led_Controller.h"
#include "Rtd_Controller.h"

#include "Rtd_Handler.h"
#include "Led_Handler.h"

Dispatch_Controller::Dispatch_Controller()
: rtd_handler(Rtd_Handler()),
  led_handler(Led_Handler()),
  begun(false),
  enabled(false)
{

}

// Slight hack: invoke static member accessor function in non-member function pointer to work with SoftTimer's bullshit
void step(Task*) {
  Dispatcher().dispatch();
}
// TODO: Tighten this loop if perf problems arise
Task stepTask(0, step);

void Dispatch_Controller::begin() {
  if(begun) {
    return;
  }
  begun = true;
  rtd_handler.begin();
  led_handler.begin();
  SoftTimer.add(&stepTask);
}

// Must define instance prior to use
Dispatch_Controller* Dispatch_Controller::instance = NULL;

Dispatch_Controller& Dispatch_Controller::getInstance() {
  if (!instance) {
    instance = new Dispatch_Controller();
    instance->begin();
  }
  return *instance;
}

Dispatch_Controller& Dispatcher() {
  return Dispatch_Controller::getInstance();
}

void Dispatch_Controller::disable() {
  enabled = false;
}

void Dispatch_Controller::enable() {
  enabled = true;
}

void logMessage(Frame& frame) {
  Serial.print(F("Begin Message on id "));
  Serial.print(frame.id);
  Serial.print(" (0x");
  Serial.print(frame.id, HEX);
  Serial.print(") ");
  Serial.print(": ");
  for(int i = 0; i < 8; i++) {
    Serial.print(frame.body[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

void Dispatch_Controller::dispatch() {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();
  logMessage(frame);
  switch(frame.id) {
    case VCU_ID:
      rtd_handler.handleMessage(frame);
      break;
    case POSITIVE_MOTOR_ID:
    case NEGATIVE_MOTOR_ID:
    case BMS_SOC_ID:
      led_handler.handleMessage(frame);
      break;
  }
  if(enabled) {
    performEnableActions(frame);
  }
}

void Dispatch_Controller::performEnableActions(Frame&) {
  // Nothing for now
}

