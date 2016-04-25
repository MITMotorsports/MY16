#ifndef RTD_CONTROLLER_H
#define RTD_CONTROLLER_H

#include <Debouncer.h>

class Rtd_Controller {
  public:
    static Rtd_Controller& getInstance();
    void begin();
    void disable();
    void enable();
    void shutdown(String reason);
    bool isEnabled();
  private:
    //Private constructors
    Rtd_Controller();
    static Rtd_Controller *instance;
    bool enabled;
    bool begun;
    void setEnablePins(uint8_t dir);
};

// Singleton accessor declaration
Rtd_Controller& RTD();

#endif // RTD_CONTROLLER_H
