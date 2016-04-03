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
    
    void generate(uint16_t seed, WORLD_SIZE size);
private:
    // Puzzles
    int try_placing_transport(uint16_t *world, int item_idx, int transport_count, int *placed_transport_count_ref, int iteration, int threshold_2, int v36);
    int DeterminePuzzleLocations(signed int iteration, int puzzle_count_to_place, uint16_t* world, int transport_count, int *placed_transport_count_ref, int blockade_count, int *placed_blockade_count_ref, int *placed_puzzle_count_ref);
    int AdditionalPuzzleLocations(int travels_to_place, uint16_t* world, int* placed_puzzles_count_ref);
    int map_get_island_orientation(int x, int y, uint16_t* map);
    int choose_puzzles_behind_blockades(uint16_t* world, uint16_t *puzzles);
    int choose_puzzles_on_islands(uint16_t* world, uint16_t *puzzles, int count);
    int choose_additional_puzzles(uint16_t* world, uint16_t *puzzles, int placed_puzzles, int total_puzzle_count);
    int map_count_stuff(uint16_t* world, int* travels, int* blockades, int *puzzles);
    int make_sure_last_puzzle_is_not_too_close_to_center(uint16_t *puzzles, int placed_puzzles);
    int find_puzzle(uint16_t* puzzles, int count, int *x, int *y);
    int place_intermediate_world_thing();
    
    // Transports
    void placeTransports(int count);
    void placeTransport();
    int placeTransportLeft();
    int placeTransportTop();
    int placeTransportBottom();
    int placeTransportRight();
};
#endif /* Map_hpp */
