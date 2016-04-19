#ifndef RTD_HANDLER_H
#define RTD_HANDLER_H

#include <Debouncer.h>
#include <PciManager.h>

#include "Can_Controller.h"
#include "Rtd_Controller.h"
#include "Led_Controller.h"

#include "Can_Ids.h"
#include "Pins.h"

class Rtd_Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    void processVcuMessage(Frame& message);
    void processSpeedMessage(Frame& message);
    void processSocMessage(Frame& message);
};

// Have to have a bunch of non-member functions cause reasons
void sendEnableRequest();
void sendDisableRequest();
bool sendEnableRequestWrapper(Task*);
void pressRtdButton();
void releaseRtdButton(unsigned long);

#endif // RTD_HANDLER_H
