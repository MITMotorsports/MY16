#include <Arduino.h>

#include "Can_Node.h"

#include "Can_Controller.h"
#include "Pins.h"

const unsigned char ANALOG_MESSAGE_ID = 0x01;
const unsigned char RPM_MESSAGE_ID = 0x10;

const float STARBOARD_THROTTLE_SCALE = 0.3643;
const float PORT_THROTTLE_SCALE = STARBOARD_THROTTLE_SCALE;
const int STARBOARD_THROTTLE_OFFSET = 0;
const int PORT_THROTTLE_OFFSET = -5;

const int RPM_MESSAGE_PERIOD = 100;
const int RPM_READING_PERIOD = 10;
const unsigned int MOVING_AVG_WIDTH = RPM_MESSAGE_PERIOD / RPM_READING_PERIOD;

const int ANALOG_MESSAGE_PERIOD = 100;

const int CLICKS_PER_REVOLUTION = 22;
const float REVOLUTIONS_PER_CLICK = 1.0 / CLICKS_PER_REVOLUTION;
const unsigned long MICROS_PER_MIN = 60000000;

unsigned int portRpms[MOVING_AVG_WIDTH];
unsigned int starboardRpms[MOVING_AVG_WIDTH];
unsigned int rpmIndex = 0;

unsigned long lastRpmTime = 0;
volatile unsigned int starboardClicks = 0;
volatile unsigned int portClicks = 0;

Task recordRpmTask(RPM_READING_PERIOD, recordRpm);
Task sendRpmCanMessageTask(RPM_MESSAGE_PERIOD, sendRpmCanMessage);
Task sendAnalogCanMessageTask(ANALOG_MESSAGE_PERIOD, sendAnalogCanMessage);

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

  SoftTimer.add(&recordRpmTask);
  SoftTimer.add(&sendAnalogCanMessageTask);
  SoftTimer.add(&sendAnalogCanMessageTask);

  resetClicksAndTimer(micros());
}

void logStarboardEncoderClick() {
  starboardClicks++;
}

void logPortEncoderClick() {
  portClicks++;
}

int truncateToByte(int val) {
  val = min(val, 255);
  val = max(val, 0);
  return val;
}

unsigned char readingToCan(const int val, const float scale, const int offset=0) {
  const float scaled_val = ((float)val) * scale;
  const int offset_val = round(scaled_val) + offset;
  const int bounded_val = truncateToByte(offset_val);
  unsigned char short_val = (unsigned char)(bounded_val);
  return short_val;
}

void sendAnalogCanMessage(Task*) {
  const int starboard_raw = analogRead(STARBOARD_THROTTLE_PIN);
  const int port_raw = analogRead(PORT_THROTTLE_PIN);
  const unsigned char starboard_scaled = readingToCan(
    starboard_raw,
    STARBOARD_THROTTLE_SCALE,
    STARBOARD_THROTTLE_OFFSET
  );
  const unsigned char port_scaled = readingToCan(
    port_raw,
    PORT_THROTTLE_SCALE,
    PORT_THROTTLE_OFFSET
  );
  Frame message = {.id=1, .body={starboard_scaled, port_scaled}, .len=2};
  CAN().write(message);
}

unsigned int toRpm(const unsigned long clicks, const unsigned long micros) {
  // IMPORTANT: don't change the order of these operations,
  // otherwise overflow might occur due to 32-bit resolution
  const float revs = clicks * REVOLUTIONS_PER_CLICK;
  const float revsPerMinute = (revs / micros) * MICROS_PER_MIN;
  return round(revsPerMinute);
}

void resetClicksAndTimer(const unsigned long curr) {
  lastRpmTime = curr;
  starboardClicks = 0;
  portClicks = 0;
}

void recordRpm(Task*) {
  // Cache values all at once for most accurate reading
  const unsigned long currTime = micros();
  const unsigned int cachedStarboardClicks = starboardClicks;
  const unsigned int cachedPortClicks = portClicks;

  // Once every ~70 minutes, micros() overflows back to zero.
  const bool timeOverflowed = currTime < lastRpmTime;

  // Go ahead and reset now so that interrupts can get back to work
  resetClicksAndTimer(currTime);

  if(timeOverflowed) {
    //Timer overflowed, do nothing this cycle
    return;
  }

  // Perform actual RPM calculations
  const unsigned long dt = currTime - lastRpmTime;
  const unsigned int starboardRpm = toRpm(cachedStarboardClicks, dt);
  const unsigned int portRpm = toRpm(cachedPortClicks, dt);

  // Record result, overwrite oldest existing record
  starboardRpms[rpmIndex] = starboardRpm;
  portRpms[rpmIndex] = portRpm;
  rpmIndex = (rpmIndex + 1) % MOVING_AVG_WIDTH;
}

void sendRpmCanMessage(Task*) {
  // Count total rpms
  unsigned long totStarboardRpm = 0;
  unsigned long totPortRpm = 0;
  for(unsigned int i = 0; i < MOVING_AVG_WIDTH; i++) {
    totStarboardRpm += starboardRpms[i];
    totPortRpm += portRpms[i];
  }

  // Average rpms
  unsigned int avgStarboardRpm = totStarboardRpm / MOVING_AVG_WIDTH;
  unsigned int avgPortRpm = totPortRpm / MOVING_AVG_WIDTH;

  // Generate and send message
  Frame rpmMessage = {
    .id=0x10,
    .body={
      highByte(avgStarboardRpm),
      lowByte(avgStarboardRpm),
      highByte(avgPortRpm),
      lowByte(avgPortRpm)
    },
    .len=4
  };
  CAN().write(rpmMessage);
}
