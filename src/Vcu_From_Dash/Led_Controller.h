#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

const int LED_SERIAL = 2;
const int LED_CLK = 3;
const int LED_LATCH = 4;

class Led_Controller {
  public:
    static Led_Controller& getInstance();
    void begin();
    void flex();
    void set_lightbar_power(unsigned char power);
    void set_lightbar_battery(unsigned char power);
  private:
    static Led_Controller *instance;
    void lightBarUpdate(unsigned char states[8]);
    void flexForwards();
    void flexBackwards();
    bool initialized;
};

// Singleton accessor declaration
Led_Controller& LED();

#endif // LED_CONTROLLER_H
