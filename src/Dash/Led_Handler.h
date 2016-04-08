#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include "Handler.h"

class Led_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    void processSpeedMessage(unsigned char message[8], int sign);
    void processBmsMessage(unsigned char message[8]);
};

#endif // LED_HANDLER_H

