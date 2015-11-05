//
//  MapScene.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "MapScene.hpp"
#include "Engine.hpp"
#include "World.hpp"

void MapScene::init(void)
{
    ticks = 0;

    for(auto it = _engine->data->_tiles.begin(); it != _engine->data->_tiles.end(); it++)
    {
        Tile *tile = *it;
        if(tile->isLocatorTile() && tile->getLocatorType() == 0)
        {
            printf("Found it %hu\n", tile->index);
            youAreHere = tile;
            break;
        }
    }
}

void MapScene::cleanup(void)
{}

bool MapScene::render(double delta)
{
//    printf("render map scene\n");
    
    const World *world = _engine->world;
    const unsigned short width = world->size.width;
    const unsigned short height = world->size.height;

    glClear(GL_COLOR_BUFFER_BIT);

    Zone *z = NULL;
    for (unsigned y=0; y < height; y++) {
        for (unsigned x=0; x < width; x++) {

            Tile *t = NULL;
            z = world->getZone(x, y);
            if (z) {
                Zone::Type type = z->getType();
                bool solved = z->isSolved();

                t = getTextureForType(type, solved);
            }
            else t = GameContext::CurrentContext()->getTile(836);

            renderMapTile(t, x, y, 1.0);
        }
    }

    if(ticks < 4)
    {
        GamePoint zoneLocation = world->zoneLocation;
        renderMapTile(youAreHere, zoneLocation.x, zoneLocation.y, 1.0);
    }

    return false;
}

bool MapScene::update(double delta)
{
    ticks ++;
    ticks %= 8;

    InputManager* InputManager = _engine->InputManager;
    World *world = _engine->world;
    const Pointf mouseLocation = InputManager->getMouseLocation();
    const float worldWidth = world->size.width;
    const float worldHeight = world->size.height;

    Pointi tileLocation = (Pointi) {
        .x = (int)floor(mouseLocation.x * worldWidth  - MAP_OFFSET),
        .y = (int)floor(mouseLocation.y * worldHeight + MAP_OFFSET)
    };

    if(InputManager->getLeftMouseDown())
    {
        Zone *zone = world->getZone(tileLocation.x, tileLocation.y);
        printf("Location: %dx%d\n", tileLocation.x, tileLocation.y);
        if(zone)
        {
            printf("Push dialog scene! ZoneType: %d\n", zone->getType());
        }
        else
        {
            _engine->popScene();
        }
    } else {
        bool mapKeyDown    = (InputManager->getKeyStates()       & InputManager::Map) != 0;
        bool mapKeyChanged = (InputManager->getKeyStateChanges() & InputManager::Map) != 0;
        if(mapKeyDown && mapKeyChanged) {
            _engine->popScene();
        }
    }
    return false;
}


Tile* MapScene::getTextureForType(Zone::Type type, bool solved)
{
    /*
     TypeEmpty          = 1,
     TypeBlockadeNorth  = 2,
     TypeBlockadeSouth  = 3,
     TypeBlockadeEast   = 4,
     TypeBlockadeWest   = 5,
     TypeTravelStart    = 6,
     TypeTravelEnd      = 7,
     TypeRoom  = 8,
     TypeLoad  = 9,
     TypeGoal  = 10,
     TypeTown  = 11,
     TypeWin   = 13,
     TypeLose  = 14,
     TypeTrade        = 15,
     TypeUse          = 16,
     TypeFind         = 17,
     TypeFindTheForce = 18,
     */
    unsigned tileID = 0xFFFF;
    switch (type) {
        case Zone::TypeEmpty: tileID = 832; break;
        case Zone::TypeTown:  tileID = 829; break;

        default:
            tileID = 832;
            break;
    }

    return _engine->data->getTile(tileID);
}

void MapScene::renderMapTile(Tile *t, float x, float y, float l)
{
    if(t == NULL) return;

    const float &factor = MAP_TO_TILE_FACTOR;
    const GLuint texture = t->getTexture(0);
    glBindTexture(GL_TEXTURE_2D, texture);

    x = x* MAP_TO_TILE_FACTOR + MAP_OFFSET;
    y = y* MAP_TO_TILE_FACTOR + MAP_OFFSET;

#ifdef GL_QUADS
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);       glVertex2f(x+0.0,  y+0.0);
    glTexCoord2f(factor, 0.0);    glVertex2f(x+factor, y+0.0);
    glTexCoord2f(factor, factor); glVertex2f(x+factor, y+factor);
    glTexCoord2f(0.0, factor);    glVertex2f(x+0.0, y+factor);
    glEnd();
#endif
}
