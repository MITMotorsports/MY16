#include "Rtd_Controller.h"

// Must define instance prior to use
Rtd_Controller* Rtd_Controller::instance = NULL;

const int MC_ENABLE_PIN = A9;

// Private constructor
Rtd_Controller::Rtd_Controller()
: enabled(false),
  begun(false)
{
  // Initialization done above
}

void Rtd_Controller::begin() {
  if(begun) {
    return;
  }
  begun = true;
  pinMode(MC_ENABLE_PIN, OUTPUT);
  digitalWrite(MC_ENABLE_PIN, LOW);
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
  digitalWrite(MC_ENABLE_PIN, HIGH);
}

void Rtd_Controller::disable() {
  enabled = false;
  digitalWrite(MC_ENABLE_PIN, LOW);
}

bool Rtd_Controller::isEnabled() {
  return enabled;
}
