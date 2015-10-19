#ifndef LED_MANAGER_H
#define LED_MANAGER_H

class Led_Manager {
  private:
    void forward_sequence();
    void backward_sequence();
  public:
    Led_Manager();
    void set_pins();
    void startup_sequence();
    void rtd_off();
    void rtd_on();
};
#endif // LED_MANAGER_H
