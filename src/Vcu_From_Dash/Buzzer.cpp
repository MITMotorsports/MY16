#include "Buzzer.h"

Buzzer::Buzzer(bool _enabled) 
: enabled(_enabled)
{
  // No initialization necessary
}

void Buzzer::begin() {
  pinMode(DRS, OUTPUT);
  Serial.println(F("Buzzer Begun"));
}

void Buzzer::enable() {
  Serial.println(F("Buzzer Enabled"));
  enabled = true;
}

void Buzzer::disable() {
  enabled = false;
  Serial.println(F("Buzzer Disabled"));
}

void Buzzer::trigger(int ms) {
  Serial.println(F("Buzzer Triggered"));
  // If the noise is annoying, don't do it
  if(!enabled) {
    return;
  }

  // Create callback for turning buzzer off
  DelayRun buzzerOffTask(ms, [] (Task*) {
    digitalWrite(DRS, LOW);
    Serial.println(F("Buzzer Stopped"));
    // False means don't execute follow-up task
    return false;
  });

  digitalWrite(DRS, HIGH);
  buzzerOffTask.startDelayed();
}
