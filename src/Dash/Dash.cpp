#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

//Magic timing library stuff
#include <PciManager.h>
#include <SoftTimer.h>
#include <Debouncer.h>
#include <DelayRun.h>

//Pin Definitions
#define LED_SERIAL 2
#define LED_CLK 3
#define LED_LATCH 4
#define RTD_LED 5
#define RTD_BUTTON 6
#define DRS 7
#define MCP_INT 9
#define MCP_CS 10

//CAN communication variables
unsigned char buttonSend[1];
unsigned char rtdStateSend[1];
#define FRONT_NODE 1
#define RTD_BUTTON_ID 2
#define STOP_BUTTON_ID 3
#define MC1 768
#define MC2 769
#define LED_ID 256

//LED variables
unsigned char zeros[8] = {0};

// Set CS to pin 10
MCP_CAN CAN0(MCP_CS);

//Task readTask(50, readCAN);

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
  // init can bus, baudrate: 500k
  if (CAN0.begin(CAN_500KBPS) == CAN_OK) 
  {
    Serial.print("can init ok!!\r\n");
  } else 
  {
    Serial.print("Can init fail!!\r\n");
  }

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

  //Defaults to not ready to drive
  rtdLed(0); //turn off RTD LED
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
      case FRONT_NODE:
        break;
    }
  }
}

//changes the state of the RTD LED
void rtdLed(int newState)
{
  digitalWrite(RTD_LED, newState);
}

void lightBarUpdate(unsigned char states[8])
{
  digitalWrite(LED_LATCH, LOW);

  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[1]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[0]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[3]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[2]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[5]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[4]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[7]);
  shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, (int)states[6]);

  digitalWrite(LED_LATCH, HIGH);
}

void flex()
{
  Serial.println("flexing");
  flexForwards();
  flexBackwards();
  flexForwards();
}

int getBinary(int i){
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
