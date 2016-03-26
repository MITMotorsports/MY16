#ifndef HANDLER_H
#define HANDLER_H

#include "Can_Ids.h"
#include "Can_Controller.h"

class Handler {
  public:
    // Pure virtual functions must be redefined in subclasses
    virtual void begin() = 0;
    virtual void handleMessage(Frame& message) = 0;
};

#endif // HANDLER_H
