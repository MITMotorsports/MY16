#include "Dispatch_Controller.h"

//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"
#include "Led_Controller.h"
#include "Rtd_Controller.h"

#include "Rtd_Handler.h"

Dispatch_Controller::Dispatch_Controller()
: rtd_handler(Rtd_Handler()),
  begun(false)
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
  // Idempotent
  if(begun) {
    return;
  }
  begun = true;
  // Start serial bus
  Serial.begin(115200);

  // Initialize LED controller and run blocking flex sequence
  LED().begin();
  LED().flex();

  // Initialize RTD state controller
  RTD().begin();

  // Initialize CAN controller
  CAN().begin();

  // Initialize rtd handler
  rtd_handler.begin();

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

void Dispatch_Controller::dispatch() {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();
  rtd_handler.handleMessage(frame);
}
