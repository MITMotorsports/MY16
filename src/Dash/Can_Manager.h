#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <mcp_can.h>

class Can_Manager {
  private:
    MCP_CAN delegate;

  public:
    Can_Manager();
    void init();

};

#endif //CAN_MANAGER_H
