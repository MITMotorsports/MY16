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
  bms_handler(Bms_Handler()),
  motor_handler(Motor_Handler()),
  begun(false),
  enabled(false)
{

}

// Slight hack: invoke static member accessor function in non-member function
// pointer to work with SoftTimer's bullshit.
// TODO: Tighten this loop if perf problems arise
void dispatchPointer(Task*) {
  Dispatcher().dispatch();
}
Task stepTask(0, dispatchPointer);

void Dispatch_Controller::begin() {
  if(begun) {
    return;
  }
  begun = true;
  rtd_handler.begin();
  bms_handler.begin();
  motor_handler.begin();
  SoftTimer.add(&stepTask);
  Serial.println("");
  Serial.println("VEHICLE_POWERED_ON");
  Serial.println("");
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
  RTD().disable();
  Frame disableMessage = { .id=DASH_ID, .body={0}};
  CAN().write(disableMessage);
  Serial.println("");
  Serial.println("VEHICLE_DISABLED");
  Serial.println("");
}

void Dispatch_Controller::enable() {
  enabled = true;
  RTD().enable();
  Frame enableMessage = { .id=DASH_ID, .body={1}};
  CAN().write(enableMessage);
  Serial.println("");
  Serial.println("VEHICLE_ENABLED");
  Serial.println("");
}

void Dispatch_Controller::dispatch() {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();
  rtd_handler.handleMessage(frame);
  bms_handler.handleMessage(frame);
  motor_handler.handleMessage(frame);
}
