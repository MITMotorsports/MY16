#ifndef STORE_CONTROLLER_H
#define STORE_CONTROLLER_H

#include <Arduino.h>

class Store_Controller {
  public:
    static Store_Controller& readInstance();

    static const int16_t SENTINAL = -32768;

    enum Wheel {FrontRight, FrontLeft, RearRight, RearLeft, WHEEL_LENGTH};
    void logSpeed(Wheel wheel, int16_t rpm);
    int16_t readSpeed(Wheel wheel);

    void logThrottle(int16_t throttle);
    int16_t readThrottle();

    void logBrake(int16_t brake);
    int16_t readBrake();

    void logBmsTemp(int16_t temp);
    int16_t readBmsTemp();

    void logBmsCurrent(int16_t current);
    int16_t readBmsCurrent();

    void logBmsVoltage(int16_t volts);
    int16_t readBmsVoltage();

    void logSoc(int16_t percent);
    int16_t readSoc();

  private:
    Store_Controller();
    static Store_Controller *instance;

    int16_t speeds[WHEEL_LENGTH];
    int16_t throttle;
    int16_t brake;
    int16_t bmsTemp;
    int16_t bmsCurrent;
    int16_t bmsVoltage;
    int16_t soc;
};

Store_Controller& Store();

#endif // STORE_CONTROLLER_H
