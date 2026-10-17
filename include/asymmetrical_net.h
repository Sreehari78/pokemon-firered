#ifndef GUARD_ASYMMETRICAL_NET_H
#define GUARD_ASYMMETRICAL_NET_H

#include "global.h"

// A clean struct to hold Player 1's live data once Player 2 unpacks it
struct TelemetryData {
    u8 mapGroup;
    u8 mapNum;
    u8 x;
    u8 y;
    u8 direction;
};

extern struct TelemetryData gP1Telemetry;

void AsymNet_Init(void);
void AsymNet_SendPing(void);
void AsymNet_CheckMailbox(void);

#endif // GUARD_ASYMMETRICAL_NET_H