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
  public:
    void begin();
    void rtd_off();
    void rtd_on();
    void motor_power(int power);
};
#endif // LED_MANAGER_H
