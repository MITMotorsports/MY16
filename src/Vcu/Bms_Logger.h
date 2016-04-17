#ifndef BMS_LOGGER_H
#define BMS_LOGGER_H

#include <Arduino.h>
#include "Base_Logger.h"

class Bms_Logger {
  public:
    Bms_Logger(Base_Logger _base);

    enum Reading {Voltage, Current, Temperature, READING_LENGTH};
    static String reading_units[READING_LENGTH];
    static String reading_names[READING_LENGTH];

    enum Dir {Max, Min, DIR_LENGTH};
    static String dir_names[DIR_LENGTH];

    void logMisc(String prefix, int32_t reading, String postfix);
    void logMiscCell(String prefix, uint8_t cell, int32_t reading, String postfix);

    // Lazy functions for laziness
    void log(Reading type, int32_t data);
    void logCell(Reading type, Dir dir, uint8_t cell, int32_t data);

  private:
    Base_Logger delegate;

};
#endif
