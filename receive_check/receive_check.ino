// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2013-11-27


#include <SPI.h>
#include "mcp_can.h"


unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

int DATA = 2;
int CLOCK = 3;
int LATCH = 4;
int RTD_LED = 5;
int RTD_BUTTON = 6;
int DRS = 7;


void setup()
{
    Serial.begin(115200);
    pinMode(DATA, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(LATCH, OUTPUT);
    pinMode(RTD_LED, OUTPUT);
    pinMode(RTD_BUTTON, INPUT);
    pinMode(DRS, OUTPUT);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}


void loop()
{
  
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        if(buf[0] == 0)
        {
          digitalWrite(led, LOW);
        }
        if(buf[0] == 1)
        {
          digitalWrite(led, HIGH);
        }

        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buf[i]);Serial.print("\t");
        }
        Serial.println();
    }
    
    if(digitalRead(RTD_BUTTON) == 1)
    {
      digitalWrite(RTD_LED, HIGH);
      digitalWrite(DRS, HIGH);
    }else
    {
      digitalWrite(RTD_LED, LOW);
      digitalWrite(DRS, LOW);
    }
    
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
