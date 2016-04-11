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

Can_Controller& CAN() {
  return Can_Controller::getInstance();
}

void Can_Controller::begin() {
  // Idempotent
  if (begun) {
    return;
  }
  begun = true;

  // Set interrupt pin.
  // DON'T set MCP_CS: mcp_can library does it for us
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
  delegate.readMsgBuf(&frame.len, frame.body);
  frame.id = delegate.getCanId();
  return frame;
}

String Can_Controller::canResponseToString(uint8_t response) {
  switch (response) {
    case CAN_OK:
      return "CAN_OK";
      break;
    case CAN_FAILINIT:
      return "CAN_FAILINIT";
      break;
    case CAN_FAILTX:
      return "CAN_FAILTX";
      break;
    case CAN_MSGAVAIL:
      return "CAN_MSGAVAIL";
      break;
    case CAN_NOMSG:
      return "CAN_NOMSG";
      break;
    case CAN_CTRLERROR:
      return "CAN_CTRLERROR";
      break;
    case CAN_GETTXBFTIMEOUT:
      return "CAN_GETTXBFTIMEOUT";
      break;
    case CAN_SENDMSGTIMEOUT:
      return "CAN_SENDMSGTIMEOUT";
      break;
    case CAN_FAIL:
      return "CAN_FAIL";
      break;
    default:
      return "UNKNOWN_ERROR";
      break;
  }
}

void Can_Controller::write(Frame f) {
  delegate.sendMsgBuf(f.id, 0, f.len, f.body);
}
