#include "Bms_Logger.h"

Bms_Logger::Bms_Logger(Base_Logger _delegate)
  : delegate(_delegate)
{
  reading_units[Voltage] = "millivolts";
  reading_units[Current] = "amps";
  reading_units[Temperature] = "degrees";

  reading_names[Voltage] = "voltage";
  reading_names[Current] = "current";
  reading_names[Temperature] = "temp";

  dir_names[Max] = "max";
  dir_names[Min] = "min";
}

void Bms_Logger::logMisc(String prefix, int32_t reading, String postfix) {
  delegate.logReadings(prefix, reading, postfix);
}

void Bms_Logger::logMiscCell(String prefix, uint8_t cell, int32_t reading, String postfix) {
  String cell_string = "cell_";
  cell_string += cell;
  delegate.logReadings(prefix, cell_string, reading, postfix);
}

void Bms_Logger::log(Reading type, int32_t data) {
  logMisc(reading_names[type], data, reading_units[type]);
}

void Bms_Logger::logCell(Reading type, Dir dir, uint8_t cell, int32_t data) {
  String concat_prefix = dir_names[dir] + "_" + reading_names[type];
  logMiscCell(concat_prefix, cell, data, reading_units[type]);
}
