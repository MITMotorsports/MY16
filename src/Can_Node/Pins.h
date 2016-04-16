#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

const int STARBOARD_THROTTLE_PIN = A0;
const int PORT_THROTTLE_PIN = A1;

const int STARBOARD_BRAKE_PIN = A2;
const int PORT_BRAKE_PIN = A3;

// TODO change once we know exactly which pins these go on
const int STARBOARD_ENCODER_PIN = 2;
const int PORT_ENCODER_PIN = 3;

#endif // PINS_H
