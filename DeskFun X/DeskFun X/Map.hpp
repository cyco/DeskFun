//
//  Map.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef Map_hpp
#define Map_hpp

#include <stdio.h>
#include "types.h"

typedef enum {
    WORLD_ITEM_INVALID = -1,
    WORLD_ITEM_NONE = 0,
    WORLD_ITEM_EMPTY = 1,
    WORLD_ITEM_TRAVEL_START = 101,
    WORLD_ITEM_TRAVEL_END = 102,
    WORLD_ITEM_ISLAND = 104,
    WORLD_ITEM_SPACEPORT = 201,
    WORLD_ITEM_BLOCK_WEST = 301,
    WORLD_ITEM_BLOCK_EAST = 302,
    WORLD_ITEM_BLOCK_NORTH = 303,
    WORLD_ITEM_BLOCK_SOUTH = 304,
    WORLD_ITEM_KEPT_FREE = 305,
    WORLD_ITEM_PUZZLE_CANDIDATE = 300,
    WORLD_ITEM_PUZZLE = 306,
} WORLD_ITEM;

class Map {
public:
    static int logging;
    
    uint16 tiles[100];
    int16 puzzles[100];
    
    Map();
    static int GetDistanceToCenter(int x, int y);
    
    void set(int x, int y, uint16 v);
    uint16 get(int x, int y);
    uint16 get(int i);
    
    void setPuzzle(int x, int y, int16 v);
    int16 getPuzzle(int x, int y);
    
    void clear();
    void print();
private:
};
#endif /* Map_hpp */
