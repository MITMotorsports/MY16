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

const uint8_t BRAKE_PUSHED_CUTOFF = 50;

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

  uint8_t analogThrottle;
  uint8_t analogBrake;
  bool plausible = true;

  if (isPlausible(message.body[STARBOARD_THROTTLE], message.body[PORT_THROTTLE])) {
    analogThrottle = min(
      message.body[STARBOARD_THROTTLE],
      message.body[PORT_THROTTLE]
    );
  }
  else {
    analogThrottle = 0;
    plausible = false;
  }

  if (isPlausible(message.body[STARBOARD_BRAKE], message.body[PORT_BRAKE])) {
    analogBrake = max(
      message.body[STARBOARD_BRAKE],
      message.body[PORT_BRAKE]
    );
  }
  else {
    analogBrake = 0;
    plausible = false;
  }

  // Brake light should operate regardless of plausibility
  if(analogBrake < BRAKE_PUSHED_CUTOFF) {
    brakeLightOff();
  } else {
    brakeLightOn();
  }

  // So should logging
  Store().logAnalogThrottle(analogThrottle);
  Store().logAnalogBrake(analogBrake);

  // But not torque
  if (!plausible) {
    Store().logOutputTorque(0);
    writeThrottleMessages(0);
    return;
  }

  // Also zero torque if brake-throttle conflict
  if (brakeThrottleConflict(analogThrottle, analogBrake)) {
    Store().logOutputTorque(0);
    writeThrottleMessages(0);
    return;
  }

  // Change from [0:255] to [0:32767]
  const int16_t throttleExtended = analogThrottle << 7;

  // Apply scaling factor and round
  const float throttleScaled = ((float)throttleExtended) * THROTTLE_SCALING_FACTOR;
  const int16_t outputTorque = (int16_t) (round(throttleScaled));

  // Log and write torque commands
  Store().logOutputTorque(outputTorque);
  writeThrottleMessages(outputTorque);
}

bool Can_Node_Handler::isPlausible(uint8_t x, uint8_t y) {
  const uint8_t max = max(x, y);
  const uint8_t min = min(x, y);

  // 10% = 255/10
  const bool plausible = (max - min) < 25;
  return plausible;
}

bool Can_Node_Handler::brakeThrottleConflict(uint8_t analogThrottle, uint8_t analogBrake) {
  bool result = false;
  if (Store().readBrakeThrottleConflict()) {
    // We recently triggered a conflict: stay in conflict mode
    // unless throttle below 5%
    if (analogThrottle < (255 * 0.05)) {
      // Remove conflict
      Store().logBrakeThrottleConflict(false);
    }
    else {
      // Don't remove conflict
      result = true;
    }
  }
  else {
    // We are not in conflict mode: only trigger conflict if
    // throttle above 25% and brake pressed
    if (analogThrottle >= (255 * 0.25)) {
      if (analogBrake >= BRAKE_PUSHED_CUTOFF) {
        Store().logBrakeThrottleConflict(true);
        result = true;
      }
    }
  }
  return result;
}

// Right motor spins backwards
void Can_Node_Handler::writeThrottleMessages(const int16_t throttle) {
  Frame leftFrame = {
    .id=LEFT_MOTOR_REQUEST_ID,
    .body={
      TORQUE_PREFIX,
      lowByte(throttle),
      highByte(throttle)
    },
    .len=3
  };
  CAN().write(leftFrame);

  int16_t neg_throttle = -throttle;
  Frame rightFrame = {
    .id=RIGHT_MOTOR_REQUEST_ID,
    .body={
      TORQUE_PREFIX,
      lowByte(neg_throttle),
      highByte(neg_throttle)
    },
    .len=3
  };
  CAN().write(rightFrame);
}
