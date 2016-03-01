#ifndef LED_MANAGER_H
#define LED_MANAGER_H

class Led_Manager {
  private:
    void forward_sequence();
    void backward_sequence();
    void lightBarUpdate(unsigned char states[8]);
    void flex();
    void flexForwards();
    void flexBackwards();
    void doStuff(unsigned char input[8], unsigned char outer, unsigned char inner);
    void rtd_off();
    void rtd_on();
  public:
    void begin();
    void set_rtd(bool value);
    void set_lightbar_power(unsigned char power);
    void set_lightbar_battery(unsigned char power);
};
#endif // LED_MANAGER_H
