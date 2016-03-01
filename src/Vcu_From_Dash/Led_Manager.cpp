#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Led_Manager.h"

#define LED_SERIAL 2
#define LED_CLK 3
#define LED_LATCH 4
#define RTD_LED 5
#define RTD_BUTTON 6
#define DRS 7
#define MCP_INT 9

unsigned char zeros[8] = {0};

unsigned char lightbar_state[8] = {0};

unsigned char battery_reading = 0;
unsigned char power_reading = 0;

bool overheat = false;

void Led_Manager::begin() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(LED_SERIAL, OUTPUT);
  pinMode(LED_CLK, OUTPUT);
  pinMode(LED_LATCH, OUTPUT);
  pinMode(RTD_LED, OUTPUT);
  pinMode(RTD_BUTTON, INPUT);
  pinMode(DRS, OUTPUT);
  pinMode(MCP_INT, INPUT);
  flex();
  rtd_off();
}

void Led_Manager::set_rtd(bool value) {
  if(value) {
    rtd_on();
  }
  else {
    rtd_off();
  }
}

void Led_Manager::rtd_off() {
  digitalWrite(RTD_LED, 0);
}

void Led_Manager::rtd_on() {
  digitalWrite(RTD_LED, 1);
}

void Led_Manager::flex()
{
  flexForwards();
  flexBackwards();
  flexForwards();
}

void Led_Manager::lightBarUpdate(unsigned char states[8])
{
  digitalWrite(LED_LATCH, LOW);

  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[1]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[0]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[3]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[2]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[5]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[4]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[7]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[6]);

  digitalWrite(LED_LATCH, HIGH);
}

unsigned int getBinary(unsigned int i){
  return 1 << (7 - (i%8));
}

void Led_Manager::set_lightbar_power(unsigned char value) {
  if(value == power_reading) {
    return;
  }
  power_reading = value;
  // Change range from [0:30] to [2:32] to simplify math
  value = value + 2;
  uint8_t num_full_bytes = value / 8;
  uint8_t remaining_bits = value % 8;
  uint8_t remaining_bits_inverse = 8 - remaining_bits;
  uint8_t full_byte = 0xFF;
  uint8_t last_byte = ~((1 << remaining_bits_inverse) - 1);

  // Still don't clobber overheat
  if (num_full_bytes == 0) {
    // All information is stored in trailing 6 bits of last_byte
    last_byte = overheat ? (last_byte | 0b11000000) : (last_byte & 0b00111111);
  }
  else {
    // First byte is all ones, so just set overheat as needed
    lightbar_state[4] = overheat ? 0b11111111 : 0b00111111;
  }

  // Skip first byte of this part
  for(int i = 1; i < num_full_bytes; i++) {
    lightbar_state[i+4] = full_byte;
  }
  lightbar_state[num_full_bytes + 4] = last_byte;

  // Zero out all high-order unused bytes
  for(int i = num_full_bytes + 5; i < 8; i++) {
    lightbar_state[i] = 0;
  }

  lightBarUpdate(lightbar_state);
}

void Led_Manager::set_lightbar_battery(unsigned char value) {
  if(value == battery_reading) {
    return;
  }
  battery_reading = value;
  uint8_t num_full_bytes = value / 8;
  uint8_t remaining_bits = value % 8;
  uint8_t remaining_bits_inverse = 8 - remaining_bits;

  uint8_t full_byte = 0xFF;
  uint8_t last_byte = ~((1 << remaining_bits_inverse) - 1);

  // Generate first (n-1) bytes of light bar
  for(int i = 0; i < num_full_bytes; i++) {
    lightbar_state[i] = full_byte;
  }

  if(num_full_bytes == 3) {
    // Last byte is partially filled, so we need to set it without clobbering overheat
    last_byte = overheat ? (last_byte | 0b00000011) : (last_byte & 0b11111100);
    lightbar_state[num_full_bytes] = last_byte;
  }
  else {
    // Most significant byte(s) are empty, so zero them out
    for(int i = num_full_bytes; i < 4; i++) {
      lightbar_state[i] = 0;
    }

    // Except still don't clobber overheat
    last_byte = overheat ? 0b00000011 : 0b00000000;
    lightbar_state[3] = last_byte;
  }

  lightBarUpdate(lightbar_state);
}

void Led_Manager::doStuff(unsigned char input[8], unsigned char outer, unsigned char inner) {
  if(inner == 8) {
    outer++;
    inner = 0;
    if(outer == 8) {
      return;
    }
  }
  input[outer] = inner;
  lightBarUpdate(input);
}

void Led_Manager::flexBackwards(){
  unsigned char input[8] = {0};
  for (int i =7; i>-1; i--){
    for (int j=7; j>-1; j--){
      input[i] = getBinary(j);
      lightBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  lightBarUpdate(zeros);
}

void Led_Manager::flexForwards(){
  unsigned char input[8] = {0};
  for (int i =0; i<8; i++){
    for (int j=0; j<8; j++){
      input[i] = getBinary(j);
      lightBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  lightBarUpdate(zeros);
}
