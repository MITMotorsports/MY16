#include <PinChangeInt.h>
#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>
#include <SoftTimer.h>

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
long unsigned int rxId;
int notSent = 1;
unsigned char buttonSend[1];
unsigned char rtdStateSend[1];
#define BUTTON_SEND_ID 2
#define VCU_ID 3
#define LED_ID 256

//LED variables
unsigned char zeros[8] = {0};

//RTD button variables
volatile int RTD_BUTTON_CHANGE = 0;

//State variables
const int NOT_RTD_SETUP = 0;
const int NOT_RTD = 1;
const int RTD_ACK = 2;
const int RTD_SETUP = 3;
const int RTD = 4;
const int SOFT_STOP = 5; 

int Dash_State = NOT_RTD; 

int startTime; //reference time

// Set CS to pin 10
MCP_CAN CAN0(MCP_CS);

//Task readTask(50, readCAN);
/***************** Prototypes *************/
void ledBarUpdate(unsigned char states[8]);
// void rtdLed(int newState);

void readCAN(Task *me);
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
  startTime = millis();

  // Not RTD setup
  // rtdLed(0); //turn off RTD LED
  ledBarUpdate(zeros); //turn off indicator LEDS
}

void flex()
{
  Serial.println("flexing");
  flexForwards();
  flexBackwards();
  flexForwards();
}

void readCAN() {
  if(CAN0.checkReceive() != CAN_MSGAVAIL) {
    //No message available: end the loop quick.
    return;
  }
  byte len;
  byte * ptr = &len;
  unsigned char rxBuf[8];
  CAN0.readMsgBuf(ptr, rxBuf);
  int id = CAN0.getCanId();
  switch(id) {
    case VCU_ID:
      switch(rxBuf[0]) {
        case 0:
          //Software Disable
          return;
      }

  }
}

int getBinary(int i){
  return 1 << (7 - (i%8));
}

void ledBarUpdate(unsigned char states[8])
{
  digitalWrite(LED_LATCH, LOW);
  for(int i = 0; i < 8; i++) {
    shiftOut(LED_SERIAL, LED_CLK, LSBFIRST, states[i]);
  }
  digitalWrite(LED_LATCH, HIGH);
}

void flexBackwards(){
  unsigned char input[8] = {0};
  for (int i =7; i>-1; i--){
    for (int j=7; j>-1; j--){
      input[i] = getBinary(j);
      ledBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  ledBarUpdate(zeros);
}

void flexForwards(){
  unsigned char input[8] = {0};
  for (int i =0; i<8; i++){
    for (int j=0; j<8; j++){
      input[i] = getBinary(j);
      ledBarUpdate(input);
      delay(10);
      input[i] = 0;
    }
  }
  ledBarUpdate(zeros);
}
