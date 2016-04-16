#include "Rtd_Light.h"

// shortcut macro
void set_rtd_light(bool status) {
  digitalWrite(RTD_LED_PIN, status);
}

// Have to statically allocate these, otherwise they will
// be purged off the stack at function exit
bool light_swap = false;
void blinkRTD(Task*) {
  set_rtd_light(light_swap);
  light_swap = !light_swap;
}
Task blinkRTDTask(RTD_PERIOD, blinkRTD);

// Called at program setup (turns light off)
void Rtd_Light::begin() {
  pinMode(RTD_LED_PIN, OUTPUT);
  set_rtd_light(false);
}

// Turn RTD light on
void Rtd_Light::enable() {
  SoftTimer.remove(&blinkRTDTask);
  set_rtd_light(true);
}

// Start blinking RTD light
void Rtd_Light::disable(int period) {
  SoftTimer.remove(&blinkRTDTask);
  light_swap = true;
  blinkRTDTask.setPeriodMs(period);
  SoftTimer.add(&blinkRTDTask);
}
