#include "Rtd_Controller.h"
#include "Pins.h"

// Must define instance prior to use
Rtd_Controller* Rtd_Controller::instance = NULL;

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
  pinMode(SHUTDOWN_CIRCUIT_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_CIRCUIT_PIN, HIGH);

  pinMode(MC_ENABLE_PIN, OUTPUT);
  pinMode(MC_ENABLE_BOOSTER_PIN, OUTPUT);

  setEnablePins(LOW);

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
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
  setEnablePins(HIGH);
}

void Rtd_Controller::disable() {
  enabled = false;
  setEnablePins(LOW);
}

void Rtd_Controller::setEnablePins(uint8_t direction) {
  if (direction == LOW) {
    // Set pin 1 and pin 4 to low, all others stay the same
    PORTK = PORTK & 0b11101101;
  }
  else if (direction == HIGH) {
    // Set pin 1 and pin 4 to high, all others stay the same
    PORTK = PORTK | 0b00010010;
  }
}

void Rtd_Controller::shutdown(String reason = "") {
  disable();
  Serial.println("");
  Serial.print("CAR_SHUTDOWN: ");
  Serial.println(reason);
  Serial.println("");
  digitalWrite(SHUTDOWN_CIRCUIT_PIN, LOW);
}

bool Rtd_Controller::isEnabled() {
  return enabled;
}
