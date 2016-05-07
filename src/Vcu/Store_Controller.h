#ifndef STORE_CONTROLLER_H
#define STORE_CONTROLLER_H

#include <Arduino.h>

class Store_Controller {
  public:
    static Store_Controller& readInstance();

    static const int16_t SENTINAL = -32768;

    enum Wheel {FrontRightWheel, FrontLeftWheel, RearRightWheel, RearLeftWheel, WHEEL_LENGTH};

    enum MotorController {LeftMotor, RightMotor, MOTOR_CONTROLLER_LENGTH};
    void logSpeed(Wheel wheel, int16_t rpm);
    int16_t readSpeed(Wheel wheel);

    void logAnalogThrottle(uint8_t throttle);
    uint8_t readAnalogThrottle();

    void logAnalogBrake(uint8_t brake);
    uint8_t readAnalogBrake();

    void logOutputTorque(int16_t torque);
    int16_t readOutputTorque();

    void logBrakeThrottleConflict(bool conflict);
    bool readBrakeThrottleConflict();

    void logMotorResponse(MotorController dir);
    bool readMotorResponse(MotorController dir);

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
    bool responses[MOTOR_CONTROLLER_LENGTH];
    uint8_t analogThrottle;
    uint8_t analogBrake;
    int16_t outputTorque;
    bool brakeThrottleConflict;
    int16_t bmsTemp;
    int16_t bmsCurrent;
    int16_t bmsVoltage;
    int16_t soc;
};

Store_Controller& Store();

#endif // STORE_CONTROLLER_H
