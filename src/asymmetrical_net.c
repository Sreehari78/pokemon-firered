#include "global.h"
#include "gba/io_reg.h"
#include "asymmetrical_net.h"

// Include the vanilla engine headers so we can access Player 1's data
#include "overworld.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"

// THE BUILD TOGGLE: 
#define IS_PLAYER_1 1

// The global struct where Player 2 will store the incoming data
struct TelemetryData gP1Telemetry;

void AsymNet_Init(void)
{
    REG_RCNT = 0;

    if (IS_PLAYER_1) 
    {
        REG_SIOCNT = 0x1000 | 0x0002 | 0x0001; // Master
    } 
    else 
    {
        REG_SIOCNT = 0x1000 | 0x0002 | 0x0000; // Slave
        REG_SIOCNT |= 0x0080;                  // Arm the listener
    }
}

// THE PACKER (Player 1)
void AsymNet_SendPing(void)
{
    if (IS_PLAYER_1) 
    {
        // SAFETY CHECK: Ensure the hardware is not currently busy sending a previous packet
        if (!(REG_SIOCNT & 0x0080)) 
        {
            u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;
            u8 mapNum   = gSaveBlock1Ptr->location.mapNum;
            
            struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
            u8 x   = playerObj->currentCoords.x;
            u8 y   = playerObj->currentCoords.y;
            u8 dir = playerObj->facingDirection;

            u32 packet = 0;
            packet |= (dir & 0x7);                 
            packet |= ((y & 0xFF) << 3);           
            packet |= ((x & 0xFF) << 11);          
            packet |= ((mapNum & 0x7F) << 19);     
            packet |= ((mapGroup & 0x3F) << 26);   

            REG_SIODATA32 = packet;
            
            // Start the transfer
            REG_SIOCNT |= 0x0080;
        }
    }
}

// THE UNPACKER (Player 2)
void AsymNet_CheckMailbox(void)
{
    if (!IS_PLAYER_1) 
    {
        if (!(REG_SIOCNT & 0x0080)) 
        {
            u32 incomingData = REG_SIODATA32;
            
            // 1. Unpack the 32-bit integer back into our clean struct variables
            gP1Telemetry.direction = incomingData & 0x7;
            gP1Telemetry.y         = (incomingData >> 3) & 0xFF;
            gP1Telemetry.x         = (incomingData >> 11) & 0xFF;
            gP1Telemetry.mapNum    = (incomingData >> 19) & 0x7F;
            gP1Telemetry.mapGroup  = (incomingData >> 26) & 0x3F;
            
            // 2. Re-arm the listener
            REG_SIOCNT |= 0x0080;
        }
    }
}