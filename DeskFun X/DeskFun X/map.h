//
//  map.h
//  reimp
//
//  Created by Christoph Leimbrock on 28/03/16.
//  Copyright © 2016 chris. All rights reserved.
//


#ifndef map_h
#define map_h
#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

uint16_t map_get(uint16_t* map, int x, int y);
void map_set(uint16_t* map, int x, int y, uint16_t v);
void map_clear(uint16_t* map);
void map_copy(uint16_t* src, uint16_t* dst);
int map_get_distance_to_center(int x, int y);
int map_get_distance_to_center_no_log(int x, int y);

void map_dump(uint16_t* world);
void map_dump_sample(uint16_t* world);
int map_equal(uint16_t* m1, uint16_t* m2);
#ifdef __cplusplus
}
#endif

#endif /* map_h */
