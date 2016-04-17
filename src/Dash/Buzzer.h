#ifndef BUZZER_H
#define BUZZER_H

#include <DelayRun.h>

const bool BUZZER_ENABLED = false;

class Buzzer {
  public:
    void begin();
    void enable();
    void disable();
    void trigger(int ms);

  private:
    bool enabled = BUZZER_ENABLED;
};

#endif // BUZZER_H
