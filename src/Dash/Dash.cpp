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

//LED variables
unsigned char zeros[8] = {0};

/***************** Prototypes *************/
// Task functions for event tasks
void RTDPressed();
void RTDReleased(unsigned long pressTimespanMs);

// Task functions for timed tasks
boolean buzzerOff(Task*);
void readCAN(Task*);

// Wrappers for tasks, triggered by task manager and execute task functions
Task runCanTask(1, readCAN);
DelayRun buzzerOffTask(1333, buzzerOff);
Debouncer debouncer(RTD_BUTTON, MODE_OPEN_ON_PUSH, RTDPressed, RTDReleased);

Led_Manager LED;
Can_Manager CAN;

// LED operations
void rtdLed(int newState);

// Light-bar operations
void lightBarUpdate(unsigned char states[8]);
void flex();
void flexForwards();
void flexBackwards();
/***************** End Prototypes *********/

void setup() {
  Serial.begin(115200);
  LED.set_pins();

  CAN.init();
  
  LED.startup_sequence();
  LED.rtd_off();

  //Defaults to not ready to drive
  lightBarUpdate(zeros); //turn off indicator LEDS

  // Start interrupt and CAN read loop
  SoftTimer.add(&runCanTask);
  PciManager.registerListener(RTD_BUTTON, &debouncer);

}

void RTDPressed() {
  //Do nothing, we only act on release
  Serial.println("RTD pressed");
}

void RTDReleased(unsigned long pressTimespanMs) {
  if(pressTimespanMs >= 1000) {
    //Soft stop, send a kill msg to VCU
     Serial.println("Sent stop message");
     CAN0.sendMsgBuf(STOP_BUTTON_ID, 0, 1, buttonSend);
     rtdLed(0);
  }
  else {
    //Regular push, send start msg to VCU and buzz
    Serial.println("Sent start message");
    CAN0.sendMsgBuf(RTD_BUTTON_ID, 0, 1, buttonSend);
    rtdLed(1);
    digitalWrite(DRS, HIGH);
    //Turn off after 1333 ms delay
    buzzerOffTask.startDelayed();
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
  // Check if pin is tripped
  if (!digitalRead(MCP_INT)) {
    // One of these two ifs is redundant...not sure which one
    if(CAN0.checkReceive() != CAN_MSGAVAIL) {
      //No message available: end the loop quick.
      return;
    }
    unsigned char len = 0;
    unsigned char *ptr = &len;
    unsigned char rxBuf[8];
    CAN0.readMsgBuf(ptr, rxBuf);
    int id = CAN0.getCanId();
    Serial.print("Message ID: ");
    Serial.print(id);
    Serial.print("\r\n");
    switch(id) {
      case FRONT_NODE_ID:
        break;
    }
  }
}

void lightBarUpdate(unsigned char states[8])
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

void flex()
{
  Serial.println("flexing");
  flexForwards();
  flexBackwards();
  flexForwards();
}

unsigned int getBinary(unsigned int i){
  return 1 << (7 - (i%8));
}

void flexBackwards(){
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

void flexForwards(){
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
