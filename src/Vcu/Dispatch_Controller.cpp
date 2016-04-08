#include "Dispatch_Controller.h"

//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"

#include "Rtd_Handler.h"

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
  if(begun) {
    return;
  }
  begun = true;
  rtd_handler.begin();
  can_node_handler.begin();
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
  Frame disableMessage = { .id=DASH_ID, .body={0}};
  CAN().write(disableMessage);
  SoftTimer.remove(&requestUpdatesTask);
  Serial.println("");
  Serial.println("VEHICLE_DISABLED");
  Serial.println("");
}

void Dispatch_Controller::enable() {
  enabled = true;
  Frame enableMessage = { .id=DASH_ID, .body={1}};
  CAN().write(enableMessage);
  SoftTimer.add(&requestUpdatesTask);
  Serial.println("");
  Serial.println("VEHICLE_ENABLED");
  Serial.println("");
}

void Dispatch_Controller::dispatch() {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Serial.println("Message available");
  Frame frame = CAN().read();
  Serial.print("Received message from id ");
  Serial.print(frame.id);
  Serial.println("");
  rtd_handler.handleMessage(frame);
  bms_handler.handleMessage(frame);
  if(enabled) {
    performEnableActions(frame);
  }
}

void Dispatch_Controller::performEnableActions(Frame& frame) {
  can_node_handler.handleMessage(frame);
  motor_handler.handleMessage(frame);
}

void Dispatch_Controller::requestUpdates() {
  motor_handler.requestAllUpdates();
}
