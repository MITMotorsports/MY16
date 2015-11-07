#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <mcp_can.h>

// Relevant CAN IDs
#define MCP_CS 10

#define DASH_ID 2
#define VCU_ID 3
#define MOTOR_ID 4

typedef struct Frame {
  unsigned int id;
  unsigned char message[8];
} Frame;

extern Frame ENABLE_REQUEST;
extern Frame DISABLE_REQUEST;

class Can_Manager {
  private:
    //MCP_CAN has no default constructor
    MCP_CAN delegate = MCP_CAN(MCP_CS);

  public:
    Can_Manager();
    void begin();
    bool msgAvailable();
    Frame read();
    void write(Frame frame);
};

#endif //CAN_MANAGER_H
