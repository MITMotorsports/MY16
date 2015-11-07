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
void processMotorMessage(unsigned char message[8]);
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
    case MOTOR_ID:
      processMotorMessage(frame.message);
      break;
  }
}

void processVcuMessage(unsigned char message[8]) {
  if(message[0]) {
    Serial.println("Received start message");
    LED.rtd_on();
    digitalWrite(DRS, HIGH);
    //Turn off after 1333 ms delay
    buzzerOffTask.startDelayed();
  }
  else {
    Serial.println("Received stop message");
    LED.rtd_off();
  }
}

void processMotorMessage(unsigned char message[8]){
  int x = 0;
}

