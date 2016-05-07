#ifndef MOTOR_HANDLER_H
#define MOTOR_HANDLER_H

#include "Handler.h"

class Motor_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
    void requestPermanentUpdates(uint16_t can_id);
    void requestSingleVoltageUpdate();
    void requestPermanentVoltageUpdate();
  private:
    void requestPermanentUpdate(uint16_t can_id, uint8_t msg_type, uint8_t time);
};
#endif // MOTOR_HANDLER_H


