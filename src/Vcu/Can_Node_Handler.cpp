#include <math.h>
#include "Can_Node_Handler.h"
#include "Dispatch_Controller.h"
#include "Rtd_Controller.h"

const int STARBOARD_THROTTLE = 1;
// TODO make this actually use both pots...
const int PORT_THROTTLE = 1;

const float THROTTLE_SCALING_FACTOR = 0.2;

const unsigned char TORQUE_PREFIX = 144; //0x90

void Can_Node_Handler::begin() {
  // No initialization needed
}

void Can_Node_Handler::handleMessage(Frame& message) {
  // Enable-only task
  if(!RTD().isEnabled()) {
    return;
  }

  // Only execute if id matches
  if(message.id != CAN_NODE_ID) {
    return;
  }
  handleThrottleMessage(message.body[STARBOARD_THROTTLE], message.body[PORT_THROTTLE]);
}

void Can_Node_Handler::handleThrottleMessage(const unsigned char starboard, const unsigned char port) {
  const unsigned char max = max(starboard, port);
  const unsigned char min = min(starboard, port);
  // TODO make this an actual generalized fn
  const bool plausible = (max - min) < 25;
  if(!plausible) {
    // Kill it early if fails plausibility check
    writeThrottleMessages(0);
    Dispatcher().disable();
  }
  else {
    // Change from [0:255] to [0:32767]
    const int16_t throttleExtended = min << 7;
    // Some slight shenanagins with casting here but whatever should be fine
    const float throttleScaled = ((float)throttleExtended) * THROTTLE_SCALING_FACTOR;
    const int16_t throttle = (int16_t) (round(throttleScaled));
    // Yet another paranoia check to make sure no overflow bullshit happens
    writeThrottleMessages(max(0, throttle));
  }
}

void Can_Node_Handler::writeThrottleMessages(const int16_t throttle) {
  const int16_t positiveThrottle = throttle;
  // TODO replace this if we decide to go back to reversed polarity
  const int16_t negativeThrottle = positiveThrottle;
  const Frame positiveFrame = {
    .id=POSITIVE_MOTOR_REQUEST_ID,
    .body={
      TORQUE_PREFIX,
      lowByte(positiveThrottle),
      highByte(positiveThrottle)
    },
    .len=3
  };
  const Frame negativeFrame = {
    .id=NEGATIVE_MOTOR_REQUEST_ID,
    .body={
      TORQUE_PREFIX,
      lowByte(negativeThrottle),
      highByte(negativeThrottle)
    },
    .len=3
  };
  CAN().write(positiveFrame);
  CAN().write(negativeFrame);
}
