#include "Can_Manager.h"

// Pin for CAN

Frame ENABLE_REQUEST = { .id=DASH_ID, .message={1}};
Frame DISABLE_REQUEST = { .id=DASH_ID, .message={0}};

Can_Manager::Can_Manager () {
  delegate = MCP_CAN(MCP_CS);
}

void Can_Manager::begin() {
  if (delegate.begin(CAN_500KBPS) == CAN_OK) 
  {
    Serial.print("can init ok!!\r\n");
  } else 
  {
    Serial.print("Can init fail!!\r\n");
  }
}

bool Can_Manager::msgAvailable() {
  return delegate.checkReceive() == CAN_MSGAVAIL;
}

Frame Can_Manager::read() {
  Frame frame;
  unsigned char len = 0;
  delegate.readMsgBuf(&len, frame.message);
  frame.id = delegate.getCanId();
  return frame;
}

void Can_Manager::write(Frame f) {
  delegate.sendMsgBuf(f.id, 0, 8, f.message);
}
