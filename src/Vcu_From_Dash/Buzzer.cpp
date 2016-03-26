#include "Buzzer.h"

Buzzer::Buzzer(bool _enabled) 
: enabled(_enabled)
{
  // No initialization necessary
}

void Buzzer::begin() {
  pinMode(DRS, OUTPUT);
}

void Buzzer::enable() {
  enabled = true;
}

void Buzzer::disable() {
  enabled = false;
}

bool buzzerOff(Task*) {
  digitalWrite(DRS, LOW);
  // False means don't execute follow-up task
  return false;
}

DelayRun buzzerOffTask(1333, buzzerOff);

void Buzzer::trigger(int ms) {
  // If the noise is annoying, don't do it
  if(!enabled) {
    return;
  }

  digitalWrite(DRS, HIGH);
  buzzerOffTask.setPeriodMs(ms);
  buzzerOffTask.startDelayed();
}
