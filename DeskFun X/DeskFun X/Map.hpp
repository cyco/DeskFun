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
    int generate(uint16_t seed, WORLD_SIZE size);
    void print();
private:
    // Puzzles
    int tryPlacingTransport(int item_idx, int transport_count, int *placed_transport_count_ref, int iteration, int threshold_2, int v36);
    int determinePuzzleLocations(signed int iteration, int puzzle_count_to_place, int transport_count, int *placed_transport_count_ref, int blockade_count, int *placed_blockade_count_ref, int *placed_puzzle_count_ref);
    int additionalPuzzleLocations(int travels_to_place, int* placed_puzzles_count_ref);
    int getIslandOrientation(int x, int y);
    int choosePuzzlesBehindBlockades();
    int choosePuzzlesOnIslands(int count);
    int chooseAdditionalPuzzles(int placed_puzzles, int total_puzzle_count);
    int countStuff(int* travels, int* blockades, int *puzzles);
    int makeSureLastPuzzleIsNotTooCloseToCenter(int placed_puzzles);
    int findPuzzle(int count, int *x, int *y);
    int placeSomethingOnTheMap();
    
    // Transports
    void placeTransports(int count);
    void placeTransport();
    int placeTransportLeft();
    int placeTransportTop();
    int placeTransportBottom();
    int placeTransportRight();
};
#endif /* Map_hpp */
