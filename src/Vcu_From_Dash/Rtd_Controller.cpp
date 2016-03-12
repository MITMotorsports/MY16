#include "Rtd_Controller.h"

// Must define instance prior to use
Rtd_Controller* Rtd_Controller::instance = NULL;

// Private constructor
Rtd_Controller::Rtd_Controller()
: enabled(false),
  buzzer(true),
  light(),
  begun(false)
{
  // Initialization done above
}

void Rtd_Controller::begin() {
  if(begun) {
    return;
  }
  begun = true;
  Serial.println(F("RTD Begun"));
  buzzer.begin();
  light.begin();
}

Rtd_Controller& Rtd_Controller::getInstance() {
  if(!instance) {
    instance = new Rtd_Controller();
    instance->begin();
  }
  return *instance;
}

Rtd_Controller& RTD() {
  return Rtd_Controller::getInstance();
}

void Rtd_Controller::enable() {
    enabled = true;
    buzzer.trigger(1333);
    light.enable();
    Serial.println(F("Vehicle enabled"));
}

void Rtd_Controller::disable() {
    enabled = false;
    light.disable();
    Serial.println(F("Vehicle disabled"));
}

bool Rtd_Controller::isEnabled() {
  return enabled;
}

void Rtd_Controller::muteBuzzer() {
  buzzer.disable();
  Serial.println(F("Buzzer Muted"));
}
