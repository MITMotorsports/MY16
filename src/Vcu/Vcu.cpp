#include "Vcu.h"

#include "Can_Controller.h"

#include "Dispatch_Controller.h"

void setup() {
  // Dispatcher does all the heavy lifting
  Dispatcher().begin();
}

