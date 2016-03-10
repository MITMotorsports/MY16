#include <mcp_can.h>
#include <SPI.h>
#include <avr/interrupt.h>
#include <math.h>

//Magic timing library stuff
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

#include "Vcu_From_Dash.h"
#include "Can_Controller.h"
#include "Led_Controller.h"
#include "Rtd_Controller.h"

#include "Rtd_Handler.h"

/***************** Prototypes *************/

// CAN gets highest priority
void readCAN(Task*);
Task readCanTask(0, readCAN);
void processVcuMessage(unsigned char message[8]);
void processSpeedMessage(unsigned char message[8], int sign);
void processBMSMessage(unsigned char message[8]);

//TODO place all handlers in a hash table keyed on CAN ID
Rtd_Handler rtd_handler = Rtd_Handler();

/***************** End Prototypes *********/

void setup() {
  Serial.begin(115200);

  // Blocking call -- will delay until this is complete
  LED().flex();

  // Now start CAN read loop and button interrupt
  // SoftTimer.add(&readCanTask);
  rtd_handler.begin();
}

// called by readCanTask
void readCAN(Task*) {
  // If no message, break early
  if(!CAN().msgAvailable()) { return; }
  Frame frame = CAN().read();
  switch(frame.id) {
    case VCU_ID:
      rtd_handler.handleMessage(frame);
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
      LED().set_lightbar_power(scaled_speed);
  }
}

void processBMSMessage(unsigned char message[8]){
  unsigned char SOC = message[0];
  // Scale SOC from [0:100] to [0:30]
  double scaling_factor = 3.33333;
  SOC = SOC / scaling_factor;
  LED().set_lightbar_battery(SOC);
}

