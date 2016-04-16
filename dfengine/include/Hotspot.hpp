//
//  Hotspot.hpp
//  DeskFun Engine
//
//  Created by Christoph Leimbrock on 11/04/16.
//
//

#ifndef Hotspot_hpp
#define Hotspot_hpp

#include <stdio.h>
typedef enum : uint32_t {
    TriggerLocation = 0x00,
    SpawnLocation = 0x01,
    ForceLocation = 0x02,
    VehicleTo = 0x03,
    VehicleBack = 0x04,
    LocatorThingy = 0x05,
    CrateItem = 0x06,
    PuzzleNPC = 0x07,
    CrateWeapon = 0x08,
    DoorIn = 0x09,
    DoorOut = 0x0A,
    Unused = 0x0B,
    Lock = 0x0C,
    Teleporter = 0x0D,
    xWingFromD = 0x0E,
    xWingToD = 0x0F
} HotspotType;

class Hotspot {
public:
    HotspotType type;
    uint16_t x;
    uint16_t y;
    int16 arg1;
    uint16_t arg2;
    
    int enabled;
};

#endif /* Hotspot_hpp */
