#include "Led_Controller.h"
#include "Pins.h"

uint16_t zeros[4] = {0,0,0,0};
uint16_t lightbar_state[4] = {0,0,0,0};

unsigned char battery_reading = 0;
unsigned char power_reading = 0;

bool overheat = false;

Led_Controller* Led_Controller::instance = NULL;

Led_Controller::Led_Controller() 
: begun(false)
{
  // Initialization done above
}

Led_Controller& Led_Controller::getInstance() {
  if(!instance) {
    instance = new Led_Controller();
    instance->begin();
  }
  return *instance;
}

Led_Controller& LED() {
  return Led_Controller::getInstance();
}

void Led_Controller::begin() {
  if(begun) {
    return;
  }
  begun = true;
  pinMode(LED_SERIAL_PIN, OUTPUT);
  pinMode(LED_CLK_PIN, OUTPUT);
  pinMode(LED_LATCH_PIN, OUTPUT);
}

void Led_Controller::flex()
{
  Serial.println("Flex begin");
  flexForwards();
  flexBackwards();
  flexForwards();
  Serial.println("Flex complete");
}

void Led_Controller::shiftOut16(uint16_t value) {
  // Ensure pins are low at start
  digitalWrite(LED_CLK_PIN, LOW);
  digitalWrite(LED_SERIAL_PIN, LOW);

  for (int i = 0; i < 16; i++) {
    uint8_t pinState;
    bool bitVal = ((value >> i) & 0x0001);
    if (bitVal) {
      pinState = HIGH;
    }
    else {
      pinState = LOW;
    }

    // Set data pin
    digitalWrite(LED_SERIAL_PIN, pinState);
    // Set clk high to send pin value
    digitalWrite(LED_CLK_PIN, HIGH);
    // Zero data and clk value to prevent bleed through
    digitalWrite(LED_CLK_PIN, LOW);
    digitalWrite(LED_SERIAL_PIN, LOW);
  }
}

void Led_Controller::lightBarUpdate(uint16_t states[4])
{
  digitalWrite(LED_LATCH_PIN, LOW);

  shiftOut16(states[0]);
  shiftOut16(states[1]);
  shiftOut16(states[2]);
  shiftOut16(states[3]);

  digitalWrite(LED_LATCH_PIN, HIGH);
}

uint16_t Led_Controller::getBinary16(uint16_t i){
  return 1 << (15 - (i%16));
}

uint16_t getOnes16(uint16_t numOnes) {
  switch(numOnes) {
    case 0:
      return 0b0000000000000000;
    case 1:
      return 0b1000000000000000;
    case 2:
      return 0b1100000000000000;
    case 3:
      return 0b1110000000000000;
    case 4:
      return 0b1111000000000000;
    case 5:
      return 0b1111100000000000;
    case 6:
      return 0b1111110000000000;
    case 7:
      return 0b1111111000000000;
    case 8:
      return 0b1111111100000000;
    case 9:
      return 0b1111111110000000;
    case 10:
      return 0b1111111111000000;
    case 11:
      return 0b1111111111100000;
    case 12:
      return 0b1111111111110000;
    case 13:
      return 0b1111111111111000;
    case 14:
      return 0b1111111111111100;
    case 15:
      return 0b1111111111111110;
    case 16:
    default:
      return 0b1111111111111111;
  }
}


void Led_Controller::flexForwards() {
  uint16_t input[4] = {0,0,0,0};
  lightBarUpdate(input);
  for(int i = 0; i < 4; i++) {
    for (int j=0; j<16; j++){
      input[i] = getBinary16(j);
      lightBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  lightBarUpdate(input);
}

void Led_Controller::flexBackwards() {
  uint16_t input[4] = {0,0,0,0};
  lightBarUpdate(input);
  for(int i = 4; i >= 0; i--) {
    for (int j = 15; j >= 0; j--){
      input[i] = getBinary16(j);
      lightBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  lightBarUpdate(input);
}

void Led_Controller::set_lightbar_overheat(bool value) {
  if(value == overheat) {
    return;
  }
  overheat = value;
  if(overheat) {
    // Make it hot
    lightbar_state[1] = (lightbar_state[1] | 0x0003);
    lightbar_state[2] = (lightbar_state[2] | 0xC000);
  }
  else {
    // Make it not
    lightbar_state[1] = (lightbar_state[1] & 0xFFFC);
    lightbar_state[2] = (lightbar_state[2] & 0x3FFF);
  }
  lightBarUpdate(lightbar_state);
}

void Led_Controller::set_lightbar_power(unsigned char value) {
  // Don't do work if not necessary
  if(value == power_reading) {
    return;
  }
  power_reading = value;

  // Change range from [0:30] to [2:32] to simplify math
  value = value + 2;

  uint16_t first_half = getOnes16(value);

  uint16_t second_half = 0;
  if (value >= 16) {
    second_half = getOnes16(value - 16);
  }

  // Set first half to not clobber overheat
  first_half = overheat ? (first_half | 0xC000) : (first_half & 0x3FFF);

  lightbar_state[2] = first_half;
  lightbar_state[3] = second_half;
  lightBarUpdate(lightbar_state);
}

void Led_Controller::set_lightbar_battery(unsigned char value) {
  // Don't do work if not necessary
  if(value == battery_reading) {
    return;
  }
  battery_reading = value;

  uint16_t first_half = getOnes16(value);

  uint16_t second_half = 0;
  if (value >= 16) {
    second_half = getOnes16(value - 16);
  }

  // Set second half to not clobber overheat
  second_half = overheat ? (second_half | 0x0003) : (second_half & 0xFFFC);

  lightbar_state[0] = first_half;
  lightbar_state[1] = second_half;
  lightBarUpdate(lightbar_state);
}

