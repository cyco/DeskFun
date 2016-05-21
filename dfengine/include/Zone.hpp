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
#include "Hotspot.hpp"

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
    void _readRawAuxiliaryData(FILE* file);
    void _readRawHotspotData(FILE* file);

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
    vector<Action*> _actions;
    Tile **_tile_ptrs;
    std::vector<Hotspot*> _hotspots;
    Hotspot ** _rawHotspots;
    bool _visited;

    void setTile(Tile *t, int x, int y, int l) {
        this->setTile(t, (GamePoint){.x=x, .y=y, .l=l});
    }

    void setTile(Tile *t, GamePoint p){
        if(p.x < 0 || p.y < 0 || p.l < 0) return;
        if(p.x >= _size.width || p.y >= _size.height || p.l >= 3) return;

        _tile_ptrs[GamePointToIndex(p, _size.width, _size.height)] = t;
    }

    void setHotspots(Hotspot **hotspots, int count){
        _hotspots.resize(count);
        for(int i=0; i < count; i++)
            _hotspots[i] = hotspots[i];
    };

    std::vector<Hotspot*> getHotspots(){
        return _hotspots;
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


