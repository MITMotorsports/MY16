#ifndef RTD_CONTROLLER_H
#define RTD_CONTROLLER_H

#include <Debouncer.h>
#include "Buzzer.h"
#include "Rtd_Light.h"

class Rtd_Controller {
  public:
    static Rtd_Controller& getInstance();
    void begin();
    void disable();
    void enable();
    bool isEnabled();
    void muteBuzzer();
  private:
    //Private constructors
    Rtd_Controller();
    static Rtd_Controller *instance;
    bool enabled;
    Buzzer buzzer;
    Rtd_Light light;
};

// Singleton accessor declaration
Rtd_Controller& RTD();

#endif // RTD_CONTROLLER_H
