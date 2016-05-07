#include "Store_Controller.h"

Store_Controller* Store_Controller::instance = NULL;

Store_Controller::Store_Controller() 
  : speeds{SENTINAL, SENTINAL, SENTINAL, SENTINAL}
  , responses{false, false}
  , analogThrottle(0)
  , analogBrake(0)
  , outputTorque(0)
  , brakeThrottleConflict(false)
  , bmsTemp(SENTINAL)
  , bmsCurrent(SENTINAL)
  , bmsVoltage(SENTINAL)
  , soc(SENTINAL)
{
  // No initialization required
}

Store_Controller& Store_Controller::readInstance() {
  if(!instance) {
    instance = new Store_Controller();
  }
  return *instance;
}

Store_Controller& Store() {
  return Store_Controller::readInstance();
}

void Store_Controller::logSpeed(const Wheel wheel, const int16_t rpm) {
  speeds[wheel] = rpm;
}
int16_t Store_Controller::readSpeed(const Wheel wheel) {
  return speeds[wheel];
}

void Store_Controller::logAnalogThrottle(const uint8_t throttle) {
  analogThrottle = throttle;
}
uint8_t Store_Controller::readAnalogThrottle() {
  return analogThrottle;
}

void Store_Controller::logAnalogBrake(const uint8_t brake) {
  analogBrake = brake;
}
uint8_t Store_Controller::readAnalogBrake() {
  return analogBrake;
}

void Store_Controller::logOutputTorque(const int16_t torque) {
  outputTorque = torque;
}
int16_t Store_Controller::readOutputTorque() {
  return outputTorque;
}

void Store_Controller::logBrakeThrottleConflict(const bool conflict) {
  brakeThrottleConflict = conflict;
}
bool Store_Controller::readBrakeThrottleConflict() {
  return brakeThrottleConflict;
}

void Store_Controller::logMotorResponse(MotorController dir) {
  responses[dir] = true;
}

bool Store_Controller::readMotorResponse(MotorController dir) {
  return responses[dir];
}

void Store_Controller::logBmsTemp(const int16_t _bmsTemp) {
  bmsTemp = _bmsTemp;
}
int16_t Store_Controller::readBmsTemp() {
  return bmsTemp;
}

void Store_Controller::logBmsCurrent(const int16_t _bmsCurrent) {
  bmsCurrent = _bmsCurrent;
}
int16_t Store_Controller::readBmsCurrent() {
  return bmsCurrent;
}

void Store_Controller::logBmsVoltage(const int16_t _bmsVoltage) {
  bmsVoltage = _bmsVoltage;
}
int16_t Store_Controller::readBmsVoltage() {
  return bmsVoltage;
}

void Store_Controller::logSoc(const int16_t _soc) {
  soc = _soc;
}
int16_t Store_Controller::readSoc() {
  return soc;
}

