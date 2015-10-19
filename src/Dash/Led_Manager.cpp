#include <SPI.h>
#include <avr/interrupt.h>

#include "Led_Manager.h"

#define LED_SERIAL 2
#define LED_CLK 3
#define LED_LATCH 4
#define RTD_LED 5
#define RTD_BUTTON 6
#define DRS 7
#define MCP_INT 9

void Led_Manager::setPins() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(LED_SERIAL, OUTPUT);
  pinMode(LED_CLK, OUTPUT);
  pinMode(LED_LATCH, OUTPUT);
  pinMode(RTD_LED, OUTPUT);
  pinMode(RTD_BUTTON, INPUT);
  pinMode(DRS, OUTPUT);
  pinMode(MCP_INT, INPUT);
}

void Led_Manager::rtd_off() {
  digitalWrite(RTD_LED, 0);
}

void Led_Manager::rtd_on() {
  digitalWrite(RTD_LED, 1);
}
