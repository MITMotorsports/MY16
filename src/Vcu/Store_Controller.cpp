#include "Store_Controller.h"

Store_Controller* Store_Controller::instance = NULL;

Store_Controller::Store_Controller() 
  : speeds{SENTINAL, SENTINAL, SENTINAL, SENTINAL}
  , throttle(SENTINAL)
  , brake(SENTINAL)
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

void Store_Controller::logThrottle(const int16_t _throttle) {
  throttle = _throttle;
}
int16_t Store_Controller::readThrottle() {
  return throttle;
}

void Store_Controller::logBrake(const int16_t _brake) {
  brake = _brake;
}
int16_t Store_Controller::readBrake() {
  return brake;
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

