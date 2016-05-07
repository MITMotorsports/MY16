#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

class Led_Controller {
  public:
    static Led_Controller& getInstance();
    void begin();
    void flex();
    void shiftOut16(uint16_t val);
    uint16_t getBinary16(uint16_t val);
    void set_lightbar_power(unsigned char power);
    void set_lightbar_battery(unsigned char power);
    void set_lightbar_overheat(bool value);
    void lightBarUpdate(uint16_t states[4]);
  private:
    Led_Controller();
    void flexForwards();
    void flexBackwards();
    static Led_Controller *instance;
    bool begun;
};

// Singleton accessor declaration
Led_Controller& LED();

#endif // LED_CONTROLLER_H
