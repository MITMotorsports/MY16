#include "Rtd_Controller.h"

// Must define instance prior to use
Rtd_Controller* Rtd_Controller::instance = NULL;

const int MC_ENABLE_PORT = 7;

// Private constructor
Rtd_Controller::Rtd_Controller()
: enabled(false),
  buzzer(false),
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
  buzzer.begin();
  light.begin();
  //TODO
  digitalWrite(MC_ENABLE_PORT, LOW);
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
    //TODO change this to whatever we set motor control enable to
    digitalWrite(MC_ENABLE_PORT, HIGH);
}

void Rtd_Controller::disable() {
    enabled = false;
    light.disable();
    //TODO change this to whatever we set motor control enable to
    digitalWrite(MC_ENABLE_PORT, LOW);
}

bool Rtd_Controller::isEnabled() {
  return enabled;
}

void Rtd_Controller::muteBuzzer() {
  buzzer.disable();
}
