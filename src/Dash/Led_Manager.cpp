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

unsigned char zeros[8] = {0};

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

void doStuff(unsigned char input[8], unsigned char outer, unsigned char inner) {
  if(inner == 8) {
    outer++;
    inner = 0;
    if(outer == 8) {
      return;
    }
  }
  input[outer] = inner;
  lightBarUpdate(inner);
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
