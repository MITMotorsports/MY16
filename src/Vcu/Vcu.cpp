#include "Vcu.h"

#include "Can_Controller.h"

#include "Dispatch_Controller.h"

void setup() {
  // Start serial bus
  Serial.begin(115200);

  // Initialize CAN controller
  CAN().begin();

  // Start dispatching CAN messages to appropriate handlers
  Dispatcher().begin();
}

