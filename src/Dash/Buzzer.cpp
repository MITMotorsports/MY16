#include "Buzzer.h"
#include "Pins.h"

Buzzer::Buzzer(bool _enabled) 
: enabled(_enabled)
{
  // No initialization necessary
}

void Buzzer::begin() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void Buzzer::enable() {
  enabled = true;
}

void Buzzer::disable() {
  enabled = false;
}

bool buzzerOff(Task*) {
  digitalWrite(BUZZER_PIN, LOW);
  // False means don't execute follow-up task
  return false;
}

DelayRun buzzerOffTask(1333, buzzerOff);

void Buzzer::trigger(int ms) {
  // If the noise is annoying, don't do it
  if(!enabled) {
    return;
  }

  digitalWrite(BUZZER_PIN, HIGH);
  buzzerOffTask.setPeriodMs(ms);
  buzzerOffTask.startDelayed();
}
