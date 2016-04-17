#include <math.h>
#include "Can_Node_Handler.h"
#include "Dispatch_Controller.h"
#include "Rtd_Controller.h"
#include "Store_Controller.h"

const int STARBOARD_THROTTLE = 0;
const int PORT_THROTTLE = 1;
const int STARBOARD_BRAKE = 2;
const int PORT_BRAKE = 3;

const float THROTTLE_SCALING_FACTOR = 0.2;

const uint8_t REGEN_CUTOFF = 12;

const uint8_t TORQUE_PREFIX = 144; //0x90

void Can_Node_Handler::brakeLightOn() {
  digitalWrite(BRAKE_LIGHT_PIN, HIGH);
}

void Can_Node_Handler::brakeLightOff() {
  digitalWrite(BRAKE_LIGHT_PIN, LOW);
}

void Can_Node_Handler::begin() {
  // No initialization needed
  pinMode(BRAKE_LIGHT_PIN, OUTPUT);
  brakeLightOff();
}

void Can_Node_Handler::handleMessage(Frame& message) {
  // Enable-only task
  if(!Dispatcher().isEnabled()) {
    return;
  }

  // Only execute if id matches
  if(message.id != CAN_NODE_ID) {
    return;
  }

  if(!plausibilityCheck(message)) {
    writeThrottleMessages(0);
    brakeLightOff();
    Dispatcher().disable();
    return;
  }

  // After philosophical discussion with kchan, it was determined that false
  // positives are preferable to false negatives for braking, but false
  // negatives are preferable to false positives for throttle.
  // Thus, we use max of brake sensors and min of throttle sensors.
  const uint8_t throttle = min(message.body[STARBOARD_THROTTLE], message.body[PORT_THROTTLE]);
  Store().logThrottle(throttle);

  // TODO re-enable when brake turned on
  // const uint8_t brake = max(message.body[STARBOARD_BRAKE], message.body[PORT_BRAKE]);
  // Store().logBrake(brake);
  //
  // if(brake < 50) {
  //   brakeLightOff();
  // } else {
  //   brakeLightOn();
  // }

  // Change from [0:255] to [0:32767]
  const int16_t throttleExtended = throttle << 7;

  // Some slight shenanagins with casting here but whatever should be fine
  const float throttleScaled = ((float)throttleExtended) * THROTTLE_SCALING_FACTOR;
  const int16_t throttleRounded = (int16_t) (round(throttleScaled));
  writeThrottleMessages(throttleRounded);
}

bool Can_Node_Handler::plausibilityCheck(Frame f) {
  bool throttle = isPlausible(f.body[STARBOARD_THROTTLE], f.body[PORT_THROTTLE]);
  bool brake = isPlausible(f.body[STARBOARD_BRAKE], f.body[PORT_BRAKE]);
  return throttle && brake;
}

bool Can_Node_Handler::isPlausible(uint8_t x, uint8_t y) {
  const uint8_t max = max(x, y);
  const uint8_t min = min(x, y);

  // 10% = 255/10
  const bool plausible = (max - min) < 25;
  return plausible;
}

void Can_Node_Handler::writeThrottleMessages(const int16_t throttle) {
  Frame frame = {
    .id=POSITIVE_MOTOR_REQUEST_ID,
    .body={
      TORQUE_PREFIX,
      lowByte(throttle),
      highByte(throttle)
    },
    .len=3
  };
  CAN().write(frame);
  frame.id = NEGATIVE_MOTOR_REQUEST_ID;
  CAN().write(frame);
}
