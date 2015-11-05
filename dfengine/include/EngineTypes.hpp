//
//  EngineTypes.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef Desktop_Adventure_EngineTypes_hpp
#define Desktop_Adventure_EngineTypes_hpp
#include <Math.h>

#pragma mark -
typedef struct {
    float x;
    float y;
} Pointf;

typedef struct {
    int x;
    int y;
} Pointi;

typedef struct {
    float width;
    float height;
} Sizef;

typedef struct {
    int width;
    int height;
}  Sizei;

typedef struct
{
    int x;
    int y;
    int l;
} GamePoint;

const extern GamePoint GamePointZero;
GamePoint GamePointAdd(GamePoint a, GamePoint b);
GamePoint GamePointSubtract(GamePoint a, GamePoint b);
GamePoint GamePointMultiply(GamePoint a, int xFactor, int yFactor);
bool GamePointsAreEqual(GamePoint a, GamePoint b);
GamePoint GamePointOnLayer(GamePoint a, int new_layer);

bool GamePointIsUnidirectionalMovement(GamePoint a);
bool GamePointInBounds(GamePoint p, int width, int  height);
int GamePointToIndex(GamePoint p, int width, int height);
#pragma mark 
#endif
