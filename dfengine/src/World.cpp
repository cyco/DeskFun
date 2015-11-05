//
//  World.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "World.hpp"

const unsigned short World::HEIGHT = 10;
const unsigned short World::WIDTH  = 10;

World::World(GameDataParser &data)
    : size({.width = WIDTH, .height = HEIGHT}), data(data)
{
    init();
}

World::World(GameDataParser &data, unsigned short width, unsigned short height)
    : size({.width = width, .height = height}), data(data)
{
    init();
}

void World::init(void)
{
    srand((unsigned int)time(NULL));

    // clear zone
    for(int i=0; i < maxZonesCount; i++) {
        zones[i] = NULL;
    }

    if(GameContext::CurrentContext()->getGameStyle() == GameStyleIndy)
    {
        // setup dagobah
        zones[(size.height+1) * size.width] = data._zones[0x05e];
        zones[(size.height+1) * size.width + 1] = data._zones[0x05f];
        zones[(size.height+2) * size.width] = data._zones[0x05d];
        zones[(size.height+2) * size.width + 1] = data._zones[0x060];
    }

    vector<Puzzle*> &puzzles = data._puzzles;
    vector<Zone*> &zones = data._zones;

    vector<Puzzle*>endPuzzles;
    copy_if(puzzles.begin(), puzzles.end(), back_inserter(endPuzzles), [](Puzzle *p) {
        return p->getType() == PuzzleTypeEnd;
    });

    PlanetType type = PlanetTypeEndor;

    vector<Zone*> townZones;
    copy_if(zones.begin(), zones.end(), back_inserter(townZones), [type](Zone *z) {
        return z->getType() == Zone::TypeTown && (GameContext::CurrentContext()->getGameStyle() == GameStyleIndy
                                                  || z->getPlanet() == type);
    });

    vector<Zone*> emptyZones;
    copy_if(zones.begin(), zones.end(), back_inserter(emptyZones), [type](Zone *z) {
        return z->getType() == Zone::TypeEmpty && (GameContext::CurrentContext()->getGameStyle() == GameStyleIndy
                                                   || z->getPlanet() == type);
    });

    for(int y=0; y < 3; y++) {
        for(int x=0; x < 3; x++) {
            Zone *z = NULL;
            if(x == 1 && y == 1) {
                z = townZones[0];
            } else {
                unsigned long index = rand () * emptyZones.size() / RAND_MAX;
                z = emptyZones[index];
                emptyZones.erase(emptyZones.begin()+index);
            }
            if(x != 0 || y != 0)
                placeZone(z, x - 1 + size.width / 2, y-1 + size.height / 2);
        }
    }

    hero.appearance = data._chars[0];
    hero.location = (GamePoint){.x=size.width / 2, .y=size.height / 2, .l=1};
}

#pragma mark -
void World::placeZone(Zone *zone, unsigned short x, unsigned short y)
{
    zones[y*size.width+x] = zone;
}

Zone* World::getZone(unsigned short x, unsigned short y) const
{
    size_t index = y*size.width+x;
    if(index < maxZonesCount)
    {
        return zones[index];
    }
    return NULL;
}

Zone* World::getZone(unsigned int zoneID)
{
    return getZone(zoneID, NULL, NULL);
}

Zone* World::getZone(unsigned int zoneID, unsigned short *outX, unsigned short *outY)
{
    for(int i=0; i < maxZonesCount; i++)
    {
        if(zones[i] && zones[i]->index == zoneID)
        {
            if(outX) *outX = i % size.width;
            if(outY) *outY = i / size.width;

            return zones[i];
        }
    }

    return NULL;
}


Zone* World::getCurrentZone(void) const
{
    return getZone(zoneLocation.x, zoneLocation.y);
}