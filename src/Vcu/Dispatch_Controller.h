#ifndef DISPATCH_CONTROLLER_H
#define DISPATCH_CONTROLLER_H

#include "Rtd_Handler.h"
#include "Can_Node_Handler.h"
#include "Bms_Handler.h"
#include "Motor_Handler.h"

class Dispatch_Controller {
  public:
    static Dispatch_Controller& getInstance();
    void begin();
    void disable();
    void enable();
    void dispatch();
    void requestUpdates();
    bool isEnabled();
  private:
    Dispatch_Controller();
    void performEnableActions(Frame& frame);
    static Dispatch_Controller *instance;
    Rtd_Handler rtd_handler;
    Can_Node_Handler can_node_handler;
    Bms_Handler bms_handler;
    Motor_Handler motor_handler;
    bool begun;
    bool enabled;
};

Dispatch_Controller& Dispatcher();

#endif // DISPATCH_CONTROLLER_H
