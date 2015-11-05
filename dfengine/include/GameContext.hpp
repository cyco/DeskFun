//
//  GameContext.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__GameContext__
#define __Desktop_Adventure__GameContext__

#include <stdio.h>
#include "TileStore.hpp"

typedef enum {
    GameStyleYoda = 0,
    GameStyleIndy = 1,
} GameStyle;

class Tile;

class GameContext
{
public:
    static GameContext* CurrentContext();

    GameContext();
    
    TileStore tileStore;
    GameStyle getGameStyle();
    const unsigned char* getPalette();
    unsigned int getTileCount();
    Tile* getTile(unsigned int tileID);

    void switchToGameStyle(GameStyle);

private:
    GameStyle style;
};

#endif /* defined(__Desktop_Adventure__GameContext__) */
