//
//  MapScene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__MapScene__
#define __Desktop_Adventure__MapScene__

#include <stdio.h>
#include "OpenGL.h"

#include "Scene.hpp"
#include "Zone.hpp"
#include "Tile.hpp"

class MapScene : public Scene
{
public:
    void init(void) override;
    void cleanup(void) override;

    bool update(double delta) override;
    bool render(double delta) override;

private:
    const double MAP_TO_TILE_FACTOR = 28.0/32.0;
    const double MAP_OFFSET         = 1.0/ 9.0;

    unsigned short ticks;
    Tile *youAreHere;

    Tile* getTextureForType(Zone::Type type, bool solved);
    void renderMapTile(Tile *t, float x, float y, float l);
};

#endif 
