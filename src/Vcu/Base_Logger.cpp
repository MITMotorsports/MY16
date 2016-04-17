#include "Base_Logger.h"

Base_Logger::Base_Logger(HardwareSerial _output) 
: output(_output)
{

}

void Base_Logger::begin() {
  output.begin(115200);
}

template <typename First, typename... Rest>
inline void Base_Logger::logReadings(const First& first, const Rest&... rest) {
  output.print(String(first));
  output.print(", ");
  logReadings(rest...);
}

template <typename Only>
inline void Base_Logger::logReadings(const Only& only) {
  output.println(String(only));
}
