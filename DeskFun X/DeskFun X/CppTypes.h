
//
//  CppTypes.h
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef CppTypes_h
#define CppTypes_h
typedef enum {
    GOAL_FALCON = 0x055,
    GOAL_GENERAL_MARUTZ = 0x067,
    GOAL_HIDDEN_FACTORY = 0x06c,
    GOAL_HAN = 0x073,
    GOAL_FIND_LEIA = 0x083,
    GOAL_IMPERIAL_BATTLE_CODE = 0x084,
    GOAL_RELAY_STATION = 0x085,
    GOAL_IMPERIAL_BATTLE_STATION = 0x086,
    GOAL_WARN_THE_REBELS = 0x087,
    GOAL_AMULET = 0x0b9,
    GOAL_RESCUE_YODA = 0x0bd,
    GOAL_LANTERN_OF_SACRED_LIGHT = 0x0c6,
    GOAL_ADEGAN_CRYSTAL = 0x0c7,
    GOAL_THREEPIOS_PARTS = 0x0c9,
    GOAL_CAR = 0x0C5,
} PUZZLE_ID;

typedef enum {
    ZONETYPE_Empty          = 1,
    ZONETYPE_BlockadeNorth  = 2,
    ZONETYPE_BlockadeSouth  = 3,
    ZONETYPE_BlockadeEast   = 4,
    ZONETYPE_BlockadeWest   = 5,
    ZONETYPE_TravelStart = 6,
    ZONETYPE_TravelEnd   = 7,
    ZONETYPE_Room  = 8,
    ZONETYPE_Load  = 9,
    ZONETYPE_Goal  = 10,
    ZONETYPE_Town  = 11,
    ZONETYPE_Win   = 13,
    ZONETYPE_Lose  = 14,
    ZONETYPE_Trade        = 15,
    ZONETYPE_Use          = 16,
    ZONETYPE_Find         = 17,
    ZONETYPE_FindTheForce = 18,
} ZONE_TYPE;

typedef enum {
    HOTSPOT_TYPE_TriggerLocation = 0,
    HOTSPOT_TYPE_SpawnLocation = 1,
    HOTSPOT_TYPE_ForceLocation = 2,
    HOTSPOT_TYPE_VehicleTo = 3,
    HOTSPOT_TYPE_VehicleBack = 4,
    HOTSPOT_TYPE_LocatorThingy = 5,
    HOTSPOT_TYPE_CrateItem = 6,
    HOTSPOT_TYPE_PuzzleNPC = 7,
    HOTSPOT_TYPE_CrateWeapon = 8,
    HOTSPOT_TYPE_DoorIn = 9,
    HOTSPOT_TYPE_DoorOut = 10,
    HOTSPOT_TYPE_Unused = 11,
    HOTSPOT_TYPE_Lock = 12,
    HOTSPOT_TYPE_Teleporter = 13,
    HOTSPOT_TYPE_xWingFromD = 14,
    HOTSPOT_TYPE_xWingToD = 15
} HOTSPOT_TYPE;

typedef enum {
    PUZZLE_TYPE_PuzzleTypeU1 = 0,
    PUZZLE_TYPE_PuzzleTypeU2 = 1,
    PUZZLE_TYPE_PuzzleTypeU3 = 2,
    PUZZLE_TYPE_PuzzleTypeEnd = 3,
    PUZZLE_TYPE_PuzzleTypeU4 = 4
} PUZZLE_TYPE;

typedef enum {
    TATOOINE = 1,
    HOTH = 2,
    ENDOR = 3
} Planet;
#endif /* CppTypes_h */
