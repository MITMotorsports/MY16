#include "Bms_Handler.h"

void Bms_Handler::begin() {
  // No initialization needed
}

void Bms_Handler::handleMessage(Frame& message) {
  switch(message.id) {
    case BMS_SUMMARY_ID:
      handleSummaryMessage(message);
      break;
    case BMS_FAULT_ID:
      break;
    case BMS_VOLTAGE_ID:
      handleVoltageMessage(message);
      break;
    case BMS_CURRENT_ID:
      handleCurrentMessage(message);
      break;
    case BMS_TEMP_ID:
      handleTempMessage(message);
      break;
    case BMS_RESISTANCE_ID:
      break;
  }
}

uint16_t Bms_Handler::mergeBytes(unsigned char low, unsigned char high) {
  uint16_t low_ext = low;
  uint16_t high_ext = high << 8;
  return low_ext + high_ext;
}

void Bms_Handler::logPackMessage(String prefix, int32_t reading, String units) {
  Serial.print("bms_");
  Serial.print(prefix);
  Serial.print(", ");
  Serial.print(reading);
  Serial.print(", ");
  Serial.print(units);
  Serial.println("");
}

void Bms_Handler::logCellMessage(String prefix, unsigned char cell, int32_t reading, String units) {
  Serial.print("bms_cell_");
  Serial.print(prefix);
  Serial.print(", cell_");
  Serial.print(cell);
  Serial.print(", ");
  Serial.print(reading);
  Serial.print(", ");
  Serial.print(units);
  Serial.println("");
}

void Bms_Handler::handleVoltageMessage(Frame& message) {
  uint16_t total_volts = mergeBytes(message.body[1], message.body[0]);
  logPackMessage("total_voltage", total_volts, "volts");
  uint16_t min_volts = message.body[2] * 100;
  uint16_t max_volts = message.body[4] * 100;
  logCellMessage("min_voltage", message.body[3], min_volts, "millivolts");
  logCellMessage("max_voltage", message.body[5], max_volts, "millivolts");
}

void Bms_Handler::handleCurrentMessage(Frame& message) {
  uint16_t total_current = mergeBytes(message.body[1], message.body[0]);
  int16_t signed_current = (int16_t) total_current;
  logPackMessage("total_current", signed_current, "amps");
}

void Bms_Handler::handleTempMessage(Frame& message) {
  unsigned char temp = message.body[0];
  logPackMessage("total_temp", temp, "degrees");
  uint16_t min_temp = message.body[2];
  uint16_t max_temp = message.body[4];
  logCellMessage("min_temp", message.body[3], min_temp, "degrees");
  logCellMessage("max_temp", message.body[5], max_temp, "degrees");
}

void Bms_Handler::handleSummaryMessage(Frame& message) {
  uint16_t total_volts = mergeBytes(message.body[1], message.body[0]);
  logPackMessage("fast_total_voltage", total_volts, "volts");

  uint16_t total_current = mergeBytes(message.body[3], message.body[2]);
  int16_t signed_current = (int16_t) total_current;
  logPackMessage("fast_total_current", signed_current, "amps");

  unsigned char temp = message.body[4];
  logPackMessage("fast_total_temp", temp, "degrees");

  unsigned char SOC = message.body[5];
  logPackMessage("fast_SOC", SOC, "percent");
}
