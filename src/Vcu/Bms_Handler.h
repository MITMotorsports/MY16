#ifndef BMS_HANDLER_H
#define BMS_HANDLER_H

#include "Handler.h"

class Bms_Handler : public Handler {
  public:
    void begin();
    void handleMessage(Frame& message);
  private:
    uint16_t mergeBytes(unsigned char low, unsigned char high);
    void logPackMessage(String prefix, int32_t val, String units);
    void logCellMessage(String prefix, unsigned char cell, int32_t val, String units);
    void handleFaultMessage(Frame& message);
    void handleVoltageMessage(Frame& message);
    void handleCurrentMessage(Frame& message);
    void handleTempMessage(Frame& message);
    void handleSummaryMessage(Frame& message);
};

#endif // BMS_HANDLER_H

