#ifndef CAN_NODE_HANDLER_H
#define CAN_NODE_HANDLER_H

#include "Handler.h"

class Can_Node_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    bool isPlausible(unsigned char max, unsigned char min);
    void handleThrottleMessage(const unsigned char starboard, const unsigned char port);
    void writeThrottleMessages(const int16_t throttle);
};

#endif // CAN_NODE_HANDLER_H
