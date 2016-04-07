//
//  Zone.h
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DAParser__Zone__
#define __DAParser__Zone__

#include <stdio.h>
#include <string.h>
#include <array>
#include <vector>
#include <string>

#include "CppTypes.h"

#include "Tile.hpp"
#include "Action.hpp"
#include "EngineTypes.hpp"
#include "GameObject.hpp"

#define packed __attribute__ ((__packed__))
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

typedef struct packed {
    HotspotType type;
    uint16_t x;
    uint16_t y;
    uint16_t arg1;
    uint16_t arg2;
} Hotspot;

typedef enum : uint16_t {
    PlanetTypeNone     = 0,
    PlanetTypeTatooine = 1,
    PlanetTypeHoth     = 2,
    PlanetTypeEndor    = 3,
    PlanetTypeDagobah  = 5,
} PlanetType;

using namespace std;

class Zone : GameObject
{
public:
    typedef enum : uint32_t {
        TypeEmpty          = 1,
        TypeBlockadeNorth  = 2,
        TypeBlockadeSouth  = 3,
        TypeBlockadeEast   = 4,
        TypeBlockadeWest   = 5,
        TypeTravelStart = 6,
        TypeTravelEnd   = 7,
        TypeRoom  = 8,
        TypeLoad  = 9,
        TypeGoal  = 10,
        TypeTown  = 11,
        TypeWin   = 13,
        TypeLose  = 14,
        TypeTrade        = 15,
        TypeUse          = 16,
        TypeFind         = 17,
        TypeFindTheForce = 18,
    } Type;

    static const char NAMES_CATEGORY[];
    static const char INLINE_MARK[];
    static const char AUXILIARY_MAKRS[];
    static const unsigned short LAYERS = 3;

    static vector<Zone*> ReadFromFile(FILE *file, range range);
    static void ReadNamesFromFile(FILE *file, range range, vector<Zone*> &zones);

    Zone(FILE* file);
    ~Zone();

    size_t write(char *buffer);

    unsigned long index;

    string getName();
    Sizei getSize();
    Type getType();
    PlanetType getPlanet();
    bool isSolved();

    void readName(FILE* file);
    
    Tile* getTile(int x, int y, int l);
    Tile* getTile(GamePoint p);

    vector<uint16> providedItemIDs;
    vector<uint16> requiredItemIDs;
    vector<uint16> puzzleNPCTileIDs;
    vector<uint16> assignedItemIDs;

private:
    string _name;
    Sizei _size;
    Type   _type;
    PlanetType _planet;
    bool _solved;

    void _readHotspots(FILE *file);
    void _readAuxiliaryData(FILE* file);
    void _readIZAX(FILE* file);
    void _readIZX2(FILE* file);
    void _readIZX3(FILE* file);
    void _readIZX4(FILE* file);
    void _readActions(FILE *file);

    size_t _writeHotspots(char *buffer);
    size_t _writeAuxiliaryData(char *buffer);
    size_t _writeActions(char *buffer);

    uint32_t auxiliaryDataLengths[4];
    char *auxiliaryData[4];
public:
    vector<Action> _actions;
    Tile **_tile_ptrs;
    std::vector<Hotspot>  _hotspotss;
    bool _visited;

    void setTile(Tile *t, int x, int y, int l) {
        this->setTile(t, (GamePoint){.x=x, .y=y, .l=l});
    }

    void setTile(Tile *t, GamePoint p){
        _tile_ptrs[GamePointToIndex(p, _size.width, _size.height)] = t;
    }

    void setHotspots(Hotspot *tiles, int count){
        _hotspotss.resize(count);
        for(int i=0; i < count; i++)
            _hotspotss[i] = tiles[i];
    };

    std::vector<Hotspot> getHotspots(){
        return _hotspotss;
    };

    bool tileWalkableAt(GamePoint p);
#pragma mark - Action Execution
private:
    unsigned short counter;
public:
    unsigned short getCounter();
    void setCounter(unsigned short newCount);

    bool initialized, justEntered, enteredByPlane;

};
#endif /* defined(__DAParser__Zone__) */


