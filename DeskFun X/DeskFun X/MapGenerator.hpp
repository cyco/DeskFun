//
//  MapGenerator.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 20/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef MapGenerator_hpp
#define MapGenerator_hpp

#include <stdio.h>
#include "types.h"
#include "YodaDocument.hpp"

class MapGenerator {
private:
    WorldSize size;

public:
    MapGenerator(WorldSize size);
    bool generate();
    ~MapGenerator();
    
    uint16 *map;
    int16 *puzzles;
    int puzzleCount;
    
private:
    void _clear();
    int _countPuzzles();
    
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
#endif /* MapGenerator_hpp */
