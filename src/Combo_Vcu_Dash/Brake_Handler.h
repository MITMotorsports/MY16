#include "Can_Controller.h"

class Brake_Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    unsigned char lastLeftReading;
    unsigned char lastRightReading;
};
