#include "Can_Controller.h"

// Must define instance prior to use
Can_Controller* Can_Controller::instance = NULL;

Can_Controller::Can_Controller () 
: begun(false)
{
  delegate = MCP_CAN(MCP_CS);
}

Can_Controller& Can_Controller::getInstance() {
  if(!instance) {
    instance = new Can_Controller();
    instance->begin();
  }
  return *instance;
}

// Lazy shortcut
Can_Controller& CAN() {
  return Can_Controller::getInstance();
}

void Can_Controller::begin() {
  if (begun) {
    return;
  }
  begun = true;
  pinMode(MCP_INT, INPUT);
  if (delegate.begin(CAN_500KBPS) != CAN_OK) {
    Serial.print(F("Error when enabling CAN"));
  }
}

bool Can_Controller::msgAvailable() {
  return delegate.checkReceive() == CAN_MSGAVAIL;
}

Frame Can_Controller::read() {
  Frame frame;
  unsigned char len = 0;
  delegate.readMsgBuf(&len, frame.body);
  frame.id = delegate.getCanId();
  return frame;
}

void Can_Controller::write(Frame f) {
  delegate.sendMsgBuf(f.id, 0, 8, f.body);
}
