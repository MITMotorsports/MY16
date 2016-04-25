#ifndef CAN_NODE_HANDLER_H
#define CAN_NODE_HANDLER_H

#include "Handler.h"

class Can_Node_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    void brakeLightOn();
    void brakeLightOff();
    void handleThrottleMessage(const uint8_t starboard, const uint8_t port);
    void handleBrakeMessage(const uint8_t starboard, const uint8_t port);

    void regenMotors(uint8_t throttle);
    void driveMotors(uint8_t throttle);
    void writeThrottleMessages(const int16_t throttle);

    bool isPlausible(uint8_t max, uint8_t min);
};

#endif // CAN_NODE_HANDLER_H
