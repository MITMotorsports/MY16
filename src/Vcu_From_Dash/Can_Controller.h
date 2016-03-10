#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include <mcp_can.h>

const int MCP_INT = 9;
const int MCP_CS = 10;

const int DASH_ID = 2;
const int VCU_ID = 3;

// 0x1e6
const int POSITIVE_MOTOR_ID = 486;
// 0x1e5
const int NEGATIVE_MOTOR_ID = 485;
// 0x626
const int BMS_SOC_ID = 1574;

typedef struct Frame {
  unsigned int id;
  unsigned char message[8];
} Frame;

extern Frame ENABLE_REQUEST;
extern Frame DISABLE_REQUEST;

class Can_Controller {
  public:
    static Can_Controller& getInstance();
    void begin();
    bool msgAvailable();
    Frame read();
    void write(Frame frame);
  private:
    //MCP_CAN has no default constructor
    MCP_CAN delegate = MCP_CAN(MCP_CS);
    Can_Controller();
    static Can_Controller *instance;
};

// Singleton accessor declaration
Can_Controller& CAN();

#endif //CAN_CONTROLLER_H
