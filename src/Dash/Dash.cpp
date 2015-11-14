#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

//Magic timing library stuff
#include <PciManager.h>
#include <SoftTimer.h>
#include <Debouncer.h>
#include <DelayRun.h>

#include "Dash.h"
#include "Can_Manager.h"
#include "Led_Manager.h"

/***************** Prototypes *************/
// Declare managers with default constructors
Led_Manager LED;
Can_Manager CAN;

// Task functions for event tasks
void RTDPressed();
void RTDReleased(unsigned long pressTimespanMs);
Debouncer debouncer(RTD_BUTTON, MODE_OPEN_ON_PUSH, RTDPressed, RTDReleased);

// Task functions for timed tasks

boolean enableFired = false;
boolean buzzerOff(Task*);
boolean enable(Task*);
DelayRun buzzerOffTask(1333, buzzerOff);
DelayRun enableTask(500, enable);

// CAN gets highest priority
void readCAN(Task*);
Task readCanTask(0, readCAN);
void processVcuMessage(unsigned char message[8]);
void processSpeedMessage(unsigned char message[8], int sign);
void processBMSMessage(unsigned char message[8]);

void blinkRTD(Task*);
Task blinkRTDTask(333, blinkRTD);
boolean RTDState = false;
/***************** End Prototypes *********/

void setup() {
  Serial.begin(115200);
  CAN.begin();
  LED.begin();

  // Start interrupt and CAN read loop
  SoftTimer.add(&readCanTask);
  PciManager.registerListener(RTD_BUTTON, &debouncer);
}

boolean enable(Task*) {
  // no need for a lock on this variable since single threaded
  enableFired = true;
  Serial.println("Sent start message");
  CAN.write(ENABLE_REQUEST);
  // False means don't execute follow-up task
  return false;
}

void RTDPressed() {
  // The enable task will fire automatically if held for >1000ms
  enableFired = false;
  enableTask.startDelayed();
}

void RTDReleased(unsigned long) {
  if(!enableFired) {
    // Wasn't held for 1000ms so handle as disable message and cancel enable handler
    SoftTimer.remove(&enableTask);
    Serial.println("Sent stop message");
    CAN.write(DISABLE_REQUEST);
  }
}

// called by buzzerOffTask
boolean buzzerOff(Task*) {
  digitalWrite(DRS, LOW);
  Serial.println("Stopped buzzer");
  // False means don't execute follow-up task
  return false;
}

// called by readCanTask
void readCAN(Task*) {
  // If no message, break early
  if(!CAN.msgAvailable()) { return; }
  Frame frame = CAN.read();
  switch(frame.id) {
    case VCU_ID:
      processVcuMessage(frame.message);
      break;
    case POSITIVE_MOTOR_ID:
      processSpeedMessage(frame.message, 1);
      break;
    case NEGATIVE_MOTOR_ID:
      processSpeedMessage(frame.message, -1);
      break;
    case BMS_SOC_ID:
      processBMSMessage(frame.message);
      break;
  }
}

void blinkRTD(Task*) {
  RTDState = !RTDState;
  LED.set_rtd(RTDState);
}

void processVcuMessage(unsigned char message[8]) {
  if(message[0]) {
    Serial.println("Received start message");
    SoftTimer.remove(&blinkRTDTask);
    LED.set_rtd(true);
    /*
       TODO re-enable when necessary
      digitalWrite(DRS, HIGH);
      Turn off after 1333 ms delay
      buzzerOffTask.startDelayed();

    */
  }
  else {
    Serial.println("Received stop message");
    RTDState = false;
    SoftTimer.add(&blinkRTDTask);
  }
}

void processSpeedMessage(unsigned char message[8], int sign){
  switch(message[0]) {
    // Speed reference
    case 48:
      unsigned char hi_order = message[2];
      unsigned char low_order = message[1];
      unsigned short concat = (hi_order << 8) + low_order;
      int16_t speed = *((int16_t*)(&concat));

      // Scale speed from [0:32767 (aka 2^15 - 1)] to [0:30]
      // This magic number is just 32767/30 rounded
      int scaling_factor = 1092;
      unsigned char scaled_speed = (speed * sign) / scaling_factor;
      LED.set_lightbar_power(scaled_speed);
  }
}

void processBMSMessage(unsigned char message[8]){
  unsigned char SOC = message[0];
  // Scale SOC from [0:100] to [0:30]
  double scaling_factor = 3.33333;
  SOC = SOC / scaling_factor;
  LED.set_lightbar_battery(SOC);
}

