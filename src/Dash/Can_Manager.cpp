#include "Can_Manager.h"

// Pin for CAN
#define MCP_CS 10

// Relevant CAN IDs
#define FRONT_NODE_ID 1
#define RTD_BUTTON_ID 2
#define STOP_BUTTON_ID 3
#define MC1_ID 485
#define MC2_ID 486

//CAN communication variables
unsigned char buttonSend[1];
unsigned char rtdStateSend[1];

void Can_Manager::init() {
  delegate = MCP_CAN(MCP_CS);
  if (delegate.begin(CAN_500KBPS) == CAN_OK) 
  {
    Serial.print("can init ok!!\r\n");
  } else 
  {
    Serial.print("Can init fail!!\r\n");
  }
}
