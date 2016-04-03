//
//  map.c
//  reimp
//
//  Created by Christoph Leimbrock on 28/03/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "map.h"


uint16_t map_get(uint16_t* map, int x, int y) {
    if(x < 0 || y > 9 || y < 0 || y > 9) return WORLD_ITEM_INVALID;
    return map[x + 10 * y];
}

void map_set(uint16_t* map, int x, int y, uint16_t item) {
    map[x + 10 * y] = item;
}

void map_clear(uint16_t* map){
    for(int i=0; i < 100; i++){
        map[i] = WORLD_ITEM_NONE;
    }
}

int map_equal(uint16_t* m1, uint16_t* m2) {
    for(int i=0; i < 100; i++){
        if(m1[i] != m2[i]){
            return 0;
        }
    }
    return 1;
}

int map_get_distance_to_center(int x, int y){
    static int distances[] = {
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5
    };
    return distances[x + 10 * y];
}

int map_get_distance_to_center_no_log(int x, int y){
    static int distances[] = {
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5
    };
    return distances[x + 10 * y];
}


void map_copy(uint16_t* src, uint16_t* dst)
{
    for(int i=0; i < 100; i++)
        dst[i] = src[i];
}

#pragma mark - printing maps
void map_dump(uint16_t* world) {
    for(int i=0; i < 100; i++) {
        if(i && i%10==0) printf("\n");
        if(world[i] == 305) printf("--- ");
        else if(world[i] == 65535) printf(" -1 ");
        else if(world[i]) printf("%03d ", world[i]);
        else printf("000 ");
    }
    printf("\n");
}

void map_dump_sample(uint16_t* world) {
    printf("    ");
    for(int i=0; i < 100; i++) {
        if(i && i%10==0) printf("\n    ");
        printf("%03u, ", world[i]);
    }
    printf("\n");
}