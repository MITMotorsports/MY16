#ifndef MOTOR_HANDLER_H
#define MOTOR_HANDLER_H

#include "Handler.h"

class Motor_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
    void requestAllUpdates();
  private:
    void processSpeedMessage(Frame& message, int sign);
    void requestSingleUpdate(unsigned char id);
};
#endif // MOTOR_HANDLER_H


