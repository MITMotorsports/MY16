#include <PinChangeInt.h>
#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

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
unsigned char len = 0;
unsigned char rxBuf[8];
unsigned char buttonSend[1];
unsigned char rtdStateSend[1];
#define BUTTON_SEND_ID 2
#define RTD_STATE_ID 3
#define LED_ID 256


//LED variables
unsigned char zeros[8] = {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0};

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

MCP_CAN CAN0(MCP_CS);                               // Set CS to pin 10

void setup()
{
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

  PCintPort::attachInterrupt(RTD_BUTTON, buttonPress, RISING);

  flex();
  startTime = millis();
}

void loop()
{
  switch (Dash_State)
  {
    case NOT_RTD_SETUP:
      //Serial.println("notRtdSetup()");
      Dash_State = notRtdSetup();
      break;
    case NOT_RTD:
      //Serial.println("notRtd()");
      Dash_State = notRtd();
      break;        
    case RTD_ACK:
      //Serial.println("waitForRtdAck()");
      Dash_State = waitForRtdAck();
      break;
    case RTD_SETUP:
      //Serial.println("rtdSetup()");
      Dash_State = rtdSetup();
      break;
    case RTD:
      //Serial.println("rtd()");
      Dash_State = rtd();
      break;
    case SOFT_STOP:
      //Serial.println("softStop()");
      Dash_State = softStop();
      break;
  }
  
}

//Initializes the NOT RTD state
int notRtdSetup()
{
  notSent =1;
  Serial.println("NOT_RTD_SETUP");
  rtdStateSend[0] = 0b0;
  CAN0.sendMsgBuf(RTD_STATE_ID, 0, 1, rtdStateSend); //Send ACK messages
  rtdLed(0); //turn off RTD LED
  ledBarUpdate(zeros); //turn off indicator LEDS
  RTD_BUTTON_CHANGE = 0;
  
  return NOT_RTD;
}

//Waits for a RTD button push to start the car
int notRtd()
{
  int next_state;
  //check for a button push to change to state 2
  if (RTD_BUTTON_CHANGE > 0)
  {
    Serial.println("press");
    RTD_BUTTON_CHANGE = 0;
    next_state = RTD_ACK;
  } else
  {
    next_state = NOT_RTD;
  }
  
  return next_state;
}


//Waits for the VCU to acknowledge the RTD request
int waitForRtdAck()
{
  int next_state;
  buttonSend[0] = 0b1;
  
  //Serial.println(millis() - startTime);
  //resend message if the VCU is not responding
  if (notSent){
    CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend);
    Serial.println("resend");
    notSent = 0;
  }
  //waitFor(500);
  //check for VCU response
  if (!digitalRead(MCP_INT))
  {
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID
    Serial.println("message");
    Serial.println(rxId);
    if (rxId == RTD_STATE_ID)
    {
      //Serial.println(rxBuf[0]);
      if (rxBuf[0] == 1)
      {
        next_state = RTD_SETUP;
        return next_state;
      }
    }  
  }
  
  next_state = RTD_ACK;
  
  return next_state;
}

//Sets up the RTD state
int rtdSetup()
{
  Serial.println("RTD_SETUP");
  rtdStateSend[0] = 0b1;
  CAN0.sendMsgBuf(RTD_STATE_ID, 0, 1, rtdStateSend); //Send ACK message
  soundTheAlarm(); //You know ;)
  rtdLed(1); //Turn on the RTD led
  RTD_BUTTON_CHANGE = 0;
  
  return RTD;
}

//Controls the normal function of the car
int rtd()
{
  int next_state;
  notSent = 1;
  //read incoming CAN messages
  if (CAN_MSGAVAIL == CAN0.checkReceive())
  {
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID

    //check for a software disable
    if (rxId == RTD_STATE_ID && rxBuf[0] == 0)
    {
      next_state = NOT_RTD_SETUP;
      return next_state;
    }

    //check for a LED message
    if (rxId == LED_ID)
    {
      ledBarUpdate(rxBuf);
    }
  }

  //check for an RTD button hold and change state if it happens
  if (RTD_BUTTON_CHANGE > 0)
  {
    delay(1000);
    if (digitalRead(RTD_BUTTON) == 1)
    {
      RTD_BUTTON_CHANGE = 0;
      next_state = SOFT_STOP;
      return next_state;
    }
  }
  
  next_state = RTD;
  
  return next_state;
}


//Waits for the VCU to acknowledge the soft stop request
int softStop()
{
  int next_state;
  buttonSend[0] = 0b10;
  
  //resend message if the VCU is not responding
  //Serial.println("Time before wait: %d", millis());
  //waitFor(500);
  //Serial.println("Time after wait: %d", millis());
  if (notSent){
     notSent = 0;
     CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend);
     Serial.println("in soft stop");
  }
    
  //check for VCU response
  if (!digitalRead(MCP_INT))
  {
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID
    
    if (rxId == RTD_STATE_ID)
    {
      if (rxBuf[0] == 0)
      {
        next_state = NOT_RTD_SETUP;
        return next_state;
      }
    }
  }

  next_state = SOFT_STOP;
  
  return next_state;
}


//Activates the DRS buzzer for 1.333sec
void soundTheAlarm()
{
  digitalWrite(DRS, HIGH);
  delay(1333);
  digitalWrite(DRS, LOW);
}


//changes the state of the RTD LED
void rtdLed(int newState)
{
  digitalWrite(RTD_LED, newState);
}

//ISR for RTD Putton Press
void buttonPress()
{
  //Serial.println("buttons");
  RTD_BUTTON_CHANGE++;
}


//Updates the state of the indicator LEDS
void ledBarUpdate(unsigned char states[8])
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

//makes the haters cry by sweeping through the LEDS like MJ sweeps across the dance floor
void flex()
{
  Serial.println("flexing");
  flexForwards();
  flexBackwards();
  flexForwards();
}

int getBinary(int i){
  switch (i){
        case 0:
          return 0b10000000;
        case 1:
          return 0b01000000;
        case 2:
          return 0b00100000;
        case 3:
          return 0b00010000;
        case 4:
          return 0b00001000;
        case 5:
          return 0b00000100;
        case 6:
          return 0b00000010;
        case 7:
          return 0b00000001;
      }
}

void flexBackwards(){
  unsigned char input[8] = {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0};
  for (int i =7; i>-1; i--){
    for (int j=7; j>-1; j--){
      input[i] = getBinary(j);
      ledBarUpdate(input);
      delay(10);
      input[i] = 0b0;
    }
  }
  ledBarUpdate(zeros);
}

void flexForwards(){
  unsigned char input[8] = {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0};
  for (int i =0; i<8; i++){
    for (int j=0; j<8; j++){
      input[i] = getBinary(j);
      ledBarUpdate(input);
      delay(10);
      input[i] = 0b0;
    }
  }
  ledBarUpdate(zeros);
}

void waitFor(int t){
  int startTime = millis();
  int currentTime = millis();
  while (currentTime - startTime < t){
    currentTime = millis();
  }
}
