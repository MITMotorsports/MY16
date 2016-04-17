#include <Arduino.h>

class Base_Logger {
  public:
    Base_Logger(HardwareSerial _output);
    void begin();

    template <typename First, typename... Rest>
    inline void logReadings(const First& first, const Rest&... rest);

    template <typename Only>
    inline void logReadings(const Only& only);

  private:
    HardwareSerial output;
};
