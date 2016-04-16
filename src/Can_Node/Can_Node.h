#ifndef CAN_NODE_H
#define CAN_NODE_H

#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>

// Prototypes
void logStarboardEncoderClick();
void logPortEncoderClick();

int truncateToByte(int val);
unsigned char readingToCan(const int val, const float scale, const int offset);

void sendRpmCanMessage(Task*);
void sendAnalogCanMessage(Task*);

unsigned int toRpm(const unsigned long clicks, const unsigned long micros);
void recordRpm(Task*);


void resetClicksAndTimer(const unsigned long curr);

#endif // CAN_NODE_H
