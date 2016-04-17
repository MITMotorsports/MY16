#include "Dispatch_Controller.h"

//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"
#include "Rtd_Controller.h"

Dispatch_Controller::Dispatch_Controller()
: rtd_handler(Rtd_Handler()),
  can_node_handler(Can_Node_Handler()),
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

void requestUpdatesPointer(Task*) {
  Dispatcher().requestUpdates();
}
Task requestUpdatesTask(500, requestUpdatesPointer);

void Dispatch_Controller::begin() {
  //Make idempotent
  if(begun) {
    return;
  }
  begun = true;

  // Start serial bus
  Serial.begin(115200);

  // Initialize controllers
  CAN().begin();
  RTD().begin();

  // Initialize handlers
  rtd_handler.begin();
  can_node_handler.begin();
  bms_handler.begin();
  motor_handler.begin();

  // Start event loop
  SoftTimer.add(&stepTask);

  //Log to DAQ
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

bool Dispatch_Controller::isEnabled() {
  return enabled;
}

void Dispatch_Controller::disable() {
  // Force idempotency
  if(!enabled) {
    return;
  }
  enabled = false;

  // Actually disable
  RTD().disable();
  SoftTimer.remove(&requestUpdatesTask);

  // Notify listeners of disable
  Frame disableMessage = { .id=VCU_ID, .body={0}, .len=1};
  CAN().write(disableMessage);

  // Log disable to DAQ
  Serial.println("");
  Serial.println("VEHICLE_DISABLED");
  Serial.println("");
}

void Dispatch_Controller::enable() {
  // Force idempotency
  if(enabled) {
    return;
  }
  enabled = true;

  // Actually enable
  RTD().enable();
  SoftTimer.add(&requestUpdatesTask);

  // Notify listeners of enable
  Frame enableMessage = { .id=VCU_ID, .body={1}, .len=1};
  CAN().write(enableMessage);

  // Log enable to DAQ
  Serial.println("");
  Serial.println("VEHICLE_ENABLED");
  Serial.println("");
}

void Dispatch_Controller::dispatch() {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();

  // Send message to each handler
  rtd_handler.handleMessage(frame);
  bms_handler.handleMessage(frame);
  can_node_handler.handleMessage(frame);
  motor_handler.handleMessage(frame);
}

void Dispatch_Controller::requestUpdates() {
  motor_handler.requestAllUpdates();
}
