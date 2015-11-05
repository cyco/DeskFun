//
//  World.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__World__
#define __Desktop_Adventure__World__

#include <stdio.h>

#include "GameDataParser.hpp"
#include "Puzzle.hpp"
#include "Zone.hpp"
#include "Hero.hpp"

#include "EngineTypes.hpp"

using namespace std;

class World
{
public:
    const static unsigned short WIDTH;
    const static unsigned short HEIGHT;

    World(GameDataParser &data);
    World(GameDataParser &data, unsigned short width, unsigned short height);

    Zone* getZone(unsigned short x, unsigned short y) const;
    Zone* getZone(unsigned int zoneID);
    Zone* getZone(unsigned int zoneID, unsigned short *outX, unsigned short *outY);

    Zone *getCurrentZone(void) const;
    void setHeroAt(unsigned short heroX, unsigned short heroY, unsigned short worldX, unsigned short worldY);

    struct {
        unsigned short width;
        unsigned short height;
    } size;

    Hero hero;
    GamePoint zoneLocation;
private:
    GameDataParser data;

    void init(void);
    Zone *end;

    const size_t maxZonesCount = 130;
    array<Zone*, 130> zones;
    array<Zone*, 4> dagobah;

    void placeZone(Zone *zone, unsigned short x, unsigned short y);

};
#endif /* defined(__Desktop_Adventure__World__) */
