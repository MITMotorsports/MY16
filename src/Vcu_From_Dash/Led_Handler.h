#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include "Handler.h"
#include "Can_Controller.h"

// 0x1e6
const int POSITIVE_MOTOR_ID = 486;
// 0x1e5
const int NEGATIVE_MOTOR_ID = 485;
// 0x626
const int BMS_SOC_ID = 1574;

class Led_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    void processSpeedMessage(unsigned char message[8], int sign);
    void processBmsMessage(unsigned char message[8]);
};

#endif // LED_HANDLER_H

