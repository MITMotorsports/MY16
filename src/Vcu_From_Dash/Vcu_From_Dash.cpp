#include "Vcu_From_Dash.h"

#include "Can_Controller.h"
#include "Led_Controller.h"
#include "Rtd_Controller.h"

#include "Dispatch_Controller.h"

void setup() {
  // Start serial bus
  Serial.begin(115200);

  // Initialize LED controller and run blocking flex sequence
  LED().begin();
  LED().flex();

  // Initialize RTD state controller
  RTD().begin();

  // Initialize CAN controller
  CAN().begin();

  // Start dispatching CAN messages to appropriate handlers
  Dispatcher().begin();
}

