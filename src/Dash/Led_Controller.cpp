#include "Led_Controller.h"
#include "Pins.h"

unsigned char zeros[8] = {0};

unsigned char lightbar_state[8] = {0};

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
  flex();
}

void Led_Controller::flex()
{
  flexForwards();
  flexBackwards();
  flexForwards();
}

void Led_Controller::lightBarUpdate(unsigned char states[8])
{
  digitalWrite(LED_LATCH_PIN, LOW);

  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[1]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[0]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[3]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[2]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[5]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[4]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[7]);
  shiftOut(LED_SERIAL_PIN, LED_CLK_PIN, LSBFIRST, states[6]);

  digitalWrite(LED_LATCH_PIN, HIGH);
}

unsigned int getBinary(unsigned int i){
  return 1 << (7 - (i%8));
}

void Led_Controller::set_lightbar_power(unsigned char value) {
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

void Led_Controller::set_lightbar_battery(unsigned char value) {
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

void Led_Controller::flexBackwards(){
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

void Led_Controller::flexForwards(){
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
