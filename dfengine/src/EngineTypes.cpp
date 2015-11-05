//
//  EngineTypes.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include <stdio.h>
#include "EngineTypes.hpp"

const GamePoint GamePointZero = (GamePoint){.x=0, .y=0, .l=0 };

GamePoint GamePointAdd(GamePoint a, GamePoint b) {
    return (GamePoint){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .l = a.l + b.l };
};

GamePoint GamePointSubtract(GamePoint a, GamePoint b){
    return (GamePoint){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .l = a.l - b.l };
}

GamePoint GamePointMultiply(GamePoint a, int xFactor, int yFactor)
{
    return (GamePoint){ .x=a.x*xFactor, .y=a.y*yFactor, .l=a.l };
}

bool GamePointsAreEqual(GamePoint a, GamePoint b)
{
    return a.x == b.x && a.y == b.y;
}

bool GamePointIsUnidirectionalMovement(GamePoint a){
    return (a.x == 0 || a.y == 0) && a.x != a.y;
}

GamePoint  GamePointOnLayer(GamePoint a, int new_layer){
    return (GamePoint){
        .x = a.x,
        .y = a.y,
        .l = new_layer };
}

bool GamePointInBounds(GamePoint p, int width, int  height){
    return p.x >= 0 && p.y >= 0 && p.x < width && p.y < height;
}

int GamePointToIndex(GamePoint p, int width, int height){
    return /*Zone::LAYERS*/ 3 * (p.y * width + p.x) + p.l;
}