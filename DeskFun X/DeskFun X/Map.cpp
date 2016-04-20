//
//  Map.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "Map.hpp"
#include "win_rand.h"

#define Message(fmt, ...) if(logging) printf(fmt, ##__VA_ARGS__);

#pragma mark - C Methods
Map::Map(){
    clear();
}

int Map::logging;

#pragma mark - Accessors
void Map::set(int x, int y, uint16 v) {
    tiles[x + 10 * y] = v;
}

uint16 Map::get(int x, int y) {
    return tiles[x + 10 * y];
}

uint16 Map::get(int i) {
    return tiles[i];
}

void Map::setPuzzle(int x, int y, int16 v) {
    puzzles[x + 10 * y] = v;
}

int16 Map::getPuzzle(int x, int y) {
    return puzzles[x + 10 * y];
}

void Map::clear(){
    for(int i=0; i < 100; i++) {
        tiles[i] = 0;
        puzzles[i] = -1;
    }
}

int Map::GetDistanceToCenter(int x, int y) {
    Message("Map::GetDistanceToCenter %dx%d\n", x, y);
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

#pragma mark -
void Map::print() {
    for(int y = 0; y < 10; y++) {
        for(int x = 0; x < 10; x++) {
            Message("%03d ", get(x,y));
        }
        Message("\n");
    }
    Message("\n");
}