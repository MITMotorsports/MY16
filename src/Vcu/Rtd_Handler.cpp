#include <Debouncer.h>
#include <PciManager.h>

#include "Rtd_Handler.h"
#include "Dispatch_Controller.h"

void Rtd_Handler::begin() {
  //No initialization necessary
}

void Rtd_Handler::handleMessage(Frame& frame) {
  if(frame.id != DASH_ID) {
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


