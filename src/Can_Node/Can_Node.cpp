#include <Arduino.h>
//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"

const int STARBOARD_THROTTLE_PORT = A0;
const double THROTTLE_SCALING_FACTOR = 0.3643;

void readCanMessage(Task*) {
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();
  Serial.print("Received message from id ");
  Serial.print(frame.id);
  Serial.println("");
}

void sendCanMessage(Task*) {
  int val = analogRead(STARBOARD_THROTTLE_PORT);
  float scaled_val = ((float)val) * THROTTLE_SCALING_FACTOR;
  unsigned char short_val = (unsigned char)(round(scaled_val));
  Serial.print("Value at port ");
  Serial.print(STARBOARD_THROTTLE_PORT);
  Serial.print(": ");
  Serial.print(short_val);
  Serial.println("");
  Frame message = {.id=1, .body={short_val}};
  return;
  CAN().write(message);
}

Task readCanTask(1, readCanMessage);
Task sendCanTask(100, sendCanMessage);

void setup() {
  Serial.begin(115200);
  CAN().begin();
  pinMode(A0, INPUT);
  SoftTimer.add(&readCanTask);
  SoftTimer.add(&sendCanTask);
}

