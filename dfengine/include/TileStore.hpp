//
//  TileStore.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__TileStore__
#define __Desktop_Adventure__TileStore__

#include <stdio.h>
#include <vector>
#include "Store.hpp"

using namespace std;

class Tile;
class TileStore : public std::vector<Tile*>, public Store
{
public:
    // Serialization & Deserialization
    const static char HEADER[];

    TileStore();
    ~TileStore();

    size_t readFromFile(FILE *file);
    size_t write(void* buffer);
    
public:
    unsigned int getTileCount();
    Tile* GetTile(unsigned id);
    unsigned RegisterTile(Tile* t);
    void UnregisterTile(Tile* t);
};

#endif /* defined(__Desktop_Adventure__TileStore__) */
