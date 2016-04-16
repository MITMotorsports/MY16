#ifndef BUZZER_H
#define BUZZER_H

#include <DelayRun.h>

class Buzzer {
  public:
    Buzzer(bool _enabled);
    void begin();
    void enable();
    void disable();
    void trigger(int ms);

  private:
    bool enabled;
};

#endif // BUZZER_H
