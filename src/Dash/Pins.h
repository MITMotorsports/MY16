#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include <pins_arduino.h>

// PD3
const int RTD_BUTTON_PIN = PD3;

// PD4
const int LED_LATCH_PIN = PD4;

// PD5
const int RTD_LED_PIN = PD5;

// PD6
const int LED_CLK_PIN = PD6;

// PD7
const int BUZZER_PIN = PD7;

// PB0 - digital pin 8 but NOT PB0 because that maps to 0
const int LED_SERIAL_PIN = 8;

#endif // PINS_H
