#ifndef RTD_LIGHT_H_
#define RTD_LIGHT_H_

#include <Arduino.h>
#include <SoftTimer.h>
#include <Task.h>

const int RTD_LED = 5;
const int RTD_PERIOD = 333;

class Rtd_Light {
  public:
    void begin();
    void enable();
    void disable(int blink_period = RTD_PERIOD);
};

#endif // RTD_LIGHT_H_
