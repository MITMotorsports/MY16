#include <Arduino.h>
//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"

const int STARBOARD_THROTTLE_PIN = A0;
const int PORT_THROTTLE_PIN = A1;
const float STARBOARD_THROTTLE_SCALE = 0.3643;
const float PORT_THROTTLE_SCALE = STARBOARD_THROTTLE_SCALE;
const int STARBOARD_THROTTLE_OFFSET = 0;
const int PORT_THROTTLE_OFFSET = -5;

int truncate_to_byte(int val) {
  val = min(val, 255);
  val = max(val, 0);
  return val;
}

unsigned char reading_to_can(const int val, const float scale, const int offset=0) {
  const float scaled_val = ((float)val) * scale;
  const int offset_val = round(scaled_val) + offset;
  const int bounded_val = truncate_to_byte(offset_val);
  const unsigned char short_val = (unsigned char)(bounded_val);
  return short_val;
}

void sendCanMessage(Task*) {
  const int starboard_raw = analogRead(STARBOARD_THROTTLE_PIN);
  const int port_raw = analogRead(PORT_THROTTLE_PIN);
  const unsigned char starboard_scaled = reading_to_can(
    starboard_raw,
    STARBOARD_THROTTLE_SCALE,
    STARBOARD_THROTTLE_OFFSET
  );
  const unsigned char port_scaled = reading_to_can(
    port_raw,
    PORT_THROTTLE_SCALE,
    PORT_THROTTLE_OFFSET
  );
  Serial.print("starboard_scaled: ");
  Serial.print(starboard_scaled);
  Serial.print(", port_scaled: ");
  Serial.print(port_scaled);
  Serial.println("");
  Frame message = {.id=1, .body={starboard_scaled, port_scaled}};
  CAN().write(message);
}

Task sendCanTask(100, sendCanMessage);

void setup() {
  Serial.begin(115200);
  CAN().begin();
  pinMode(STARBOARD_THROTTLE_PIN, INPUT);
  pinMode(PORT_THROTTLE_PIN, INPUT);
  Serial.println("Successfully setup");
  SoftTimer.add(&sendCanTask);
}
