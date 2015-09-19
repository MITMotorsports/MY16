#include <PinChangeInt.h>
#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>

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
int Dash_State = 0; //0 = NOT_RTD
//1 = RTD_ACK
//2 = RTD
//3 = Soft_Stop

MCP_CAN CAN0(MCP_CS);                               // Set CS to pin 10

void setup()
{
  Serial.begin(115200);
  // init can bus, baudrate: 500k
  if (CAN0.begin(CAN_500KBPS) == CAN_OK) 
  {
  //Serial.print("can init ok!!\r\n");
  } else 
  {
    //Serial.print("Can init fail!!\r\n");
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
}

void loop()
{
  switch (Dash_State)
  {
    case 0:
      notRtd();
      break;
    case 1:
      waitForRtdAck();
      break;
    case 2:
      rtd();
      break;
    case 3:
      softStop();
      break;
  }
}

//Waits for a RTD button push to start the car
void notRtd()
{
  //Serial.println("not RTD");
  rtdStateSend[0] = 0b0;
  CAN0.sendMsgBuf(RTD_STATE_ID, 0, 1, rtdStateSend); //Send ACK messages
  rtdLed(0); //turn off RTD LED
  ledBarUpdate(zeros); //turn off indicator LEDS
  RTD_BUTTON_CHANGE = 0;

  //check for a button push to change to state 1
  while (Dash_State == 0)
  {
    if (RTD_BUTTON_CHANGE > 0)
    {
      Dash_State = 1;
      RTD_BUTTON_CHANGE = 0;
    }
  }
}


//Waits for the VCU to acknowledge the RTD request
void waitForRtdAck()
{
  //Serial.println("RTD ACK");
  buttonSend[0] = 0b1;
  CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend); //Send ACK messages
  int startTime = millis(); //Set start reference time

  //main state loop
  while (Dash_State == 1)
  {
    //check for VCU response
    if (!digitalRead(MCP_INT))
    {
      CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
      //Serial.println("message");
      //Serial.println(rxId);
      if (rxId == RTD_STATE_ID)
      {
        //Serial.println(rxBuf[0]);
        if (rxBuf[0] == 1)
        {
          Dash_State = 2;
        }
      }
    }
    
    //Serial.println(millis() - startTime);
    //resend message if the VCU is not responding
    if (millis() - startTime > 500 && Dash_State == 1 && CAN_NOMSG == CAN0.checkReceive())
    {
      //Serial.println("resend");
      CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend);
      startTime = millis();
    }
  }
}


//Controls the normal function of the car
void rtd()
{
  //Serial.println("RTD");
  rtdStateSend[0] = 0b1;
  CAN0.sendMsgBuf(RTD_STATE_ID, 0, 1, rtdStateSend); //Send ACK message
  soundTheAlarm(); //You know ;)
  rtdLed(1); //Turn on the RTD led
  RTD_BUTTON_CHANGE = 0;

  while (Dash_State == 2)
  {
    //read incoming CAN messages
    if (CAN_MSGAVAIL == CAN0.checkReceive())
    {
      CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID

      //check for a software disable
      if (rxId == RTD_STATE_ID && rxBuf[0] == 0)
      {
        Dash_State = 0;
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
        Dash_State = 3;
        RTD_BUTTON_CHANGE = 0;
      }
    }
  }
}


//Waits for the VCU to acknowledge the soft stop request
void softStop()
{
  //Serial.println("Soft stop");
  buttonSend[0] = 0b10;
  CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend); //Send ACK messages
  int startTime = millis(); //Set start reference time

  //main start loop
  while (Dash_State == 3)
  {
    //check for VCU response
    if (!digitalRead(MCP_INT))
    {
      CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
      
      if (rxId == RTD_STATE_ID)
      {
        if (rxBuf[0] == 0)
        {
          Dash_State = 0;
        }
      }
    }

    //resend message if the VCU is not responding
    if (millis() - startTime > 500 && Dash_State == 3 && CAN_NOMSG == CAN0.checkReceive())
    {
      CAN0.sendMsgBuf(BUTTON_SEND_ID, 0, 1, buttonSend);
      startTime = millis();
    }
  }
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
  //Serial.println("flexing");
  unsigned char flexor[8] = {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0};
  for (int i = 1; i < 63; i++)
  {
    //Serial.println(i);
    for (int j = i - 1; j <= i + 1; j++)
    {
      //Serial.println(j);
      int flexorInd = int(i / 8);
      byte flexorVal = j - flexorInd * 8;
      flexor[flexorInd] = flexorVal;
    }
    ledBarUpdate(flexor);
    memset(flexor, 0b0, sizeof(flexor));
    delay(10);
  }
  for (int i = 62; i > 0; i--)
  {
    for (int j = i - 1; j <= i + 1; j++)
    {
      int flexorInd = int(i / 8);
      byte flexorVal = j - flexorInd * 8;
      flexor[flexorInd] = flexorVal;
    }
    ledBarUpdate(flexor);
    memset(flexor, 0b0, sizeof(flexor));
    delay(10);
  }
  ledBarUpdate(zeros);
}
