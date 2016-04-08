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
  frame.id = delegate.getCanId();
  delegate.readMsgBuf(&len, frame.body);
  // int result = delegate.readMsgBuf(&len, frame.body);
  // Serial.print("Msg RECV on id ");
  // Serial.print(frame.id);
  // Serial.print(", result: ");
  // Serial.print(canResponseToString(result));
  // Serial.print(", timestamp: ");
  // Serial.print(millis());
  // Serial.println("");
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
  // int result = delegate.sendMsgBuf(f.id, 0, 8, f.body);
  // Serial.print("Msg send on id ");
  // Serial.print(f.id);
  // Serial.print(", result: ");
  // Serial.print(canResponseToString(result));
  // Serial.print(", timestamp: ");
  // Serial.print(millis());
  // Serial.println("");
  delegate.sendMsgBuf(f.id, 0, 8, f.body);
}

