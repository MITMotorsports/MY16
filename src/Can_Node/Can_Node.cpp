#include <Arduino.h>
//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Can_Controller.h"

// Pins
const int STARBOARD_THROTTLE_PIN = A0;
const int PORT_THROTTLE_PIN = A1;
// TODO change once we know exactly which pins these go on
const int STARBOARD_ENCODER_PIN = 2;
const int PORT_ENCODER_PIN = 3;

const float STARBOARD_THROTTLE_SCALE = 0.3643;
const float PORT_THROTTLE_SCALE = STARBOARD_THROTTLE_SCALE;
const int STARBOARD_THROTTLE_OFFSET = 0;
const int PORT_THROTTLE_OFFSET = -5;

const float REVOLUTIONS_PER_CLICK = 1.0 / 22.0;
const unsigned long MICROS_PER_MIN = 60000000;

unsigned long lastTime = 0;

volatile unsigned int starboardClicks = 0;
volatile unsigned int portClicks = 0;

Frame rpmMessage = {
  .id=0x10,
  .body={0,0,0,0},
  .len=4
};

// Prototypes
void logStarboardEncoderClick();
void logPortEncoderClick();

void sendCanMessage(Task*);
Task sendCanTask(100, sendCanMessage);

void integrateEncoderReadings(Task*);
Task integrateEncoderReadingsTask(50, integrateEncoderReadings);

// Implementation
void setup() {
  Serial.begin(115200);
  CAN().begin();

  pinMode(STARBOARD_THROTTLE_PIN, INPUT);
  pinMode(PORT_THROTTLE_PIN, INPUT);
  pinMode(STARBOARD_ENCODER_PIN, INPUT_PULLUP);
  pinMode(PORT_ENCODER_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(STARBOARD_ENCODER_PIN), logPortEncoderClick, RISING);
  attachInterrupt(digitalPinToInterrupt(PORT_ENCODER_PIN), logStarboardEncoderClick, RISING);

  SoftTimer.add(&sendCanTask);
  SoftTimer.add(&integrateEncoderReadingsTask);

  lastTime = micros();
}

void logStarboardEncoderClick() {
  starboardClicks++;
}

void logPortEncoderClick() {
  portClicks++;
}

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
  Frame message = {.id=1, .body={starboard_scaled, port_scaled}};
  CAN().write(message);
}

inline unsigned int toRpm(const unsigned long clicks, const unsigned long mics) {
  const float revs = clicks * REVOLUTIONS_PER_CLICK;
  const float revsPerMicro = revs / mics;
  const float revsPerMinute = revsPerMicro * MICROS_PER_MIN;
  return round(revsPerMinute);
}

inline void sendRpmMessage(const unsigned int starboardRpm, const unsigned int portRpm) {
  rpmMessage.body[0] = highByte(starboardRpm);
  rpmMessage.body[1] = lowByte(starboardRpm);
  rpmMessage.body[2] = highByte(portRpm);
  rpmMessage.body[3] = lowByte(portRpm);
  CAN().write(rpmMessage);
}

inline void resetClicksAndTimer(const unsigned long curr) {
  lastTime = curr;
  starboardClicks = 0;
  portClicks = 0;
}

void integrateEncoderReadings(Task*) {
  // Cache values all at once for most accurate reading
  const unsigned long currTime = micros();
  const unsigned int cachedStarboardClicks = starboardClicks;
  const unsigned int cachedPortClicks = portClicks;

  // Once every ~70 minutes, micros() overflows back to zero.
  const bool timeNotOverflowed = currTime >= lastTime;

  // Go ahead and reset now so that interrupts can get back to work
  resetClicksAndTimer(currTime);

  // Perform RPM calculations unless timer overflowed this cycle
  if(timeNotOverflowed) {
    const unsigned long dt = currTime - lastTime;
    const unsigned int starboardRpm = toRpm(cachedStarboardClicks, dt);
    const unsigned int portRpm = toRpm(cachedPortClicks, dt);
    sendRpmMessage(starboardRpm, portRpm);
  }
}
