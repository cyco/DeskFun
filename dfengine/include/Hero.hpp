//
//  Hero.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__Hero__
#define __Desktop_Adventure__Hero__

#include <stdio.h>
#include "GameObject.hpp"
#include "EngineTypes.hpp"

#include "Zone.hpp"
#include "Char.hpp"

typedef enum {
    DirectionNorth  = 1<<0,
    DirectionSouth  = 1<<1,
    DirectionEast   = 1<<2,
    DirectionWest   = 1<<3,
} DirectionMask;

class Renderer;
class GameState;
class Hero : GameObject
{
public:
    const static int MAX_HEALTH = 300;

    Hero();

    void face(float direction);
    bool move(GamePoint relative, Zone &zone);
    bool moveTo(GamePoint absolute, Zone &zone);

    bool canAttack();
    bool attack();

    void tick(unsigned ticks);
    void render(Pointf offset, Renderer *renderer);

    bool isDragging;

    Char *appearance;

    void addItem(Tile *item);
    void removeItem(Tile *item);
    bool hasItem(Tile *item);

private:
    bool _doMove(GamePoint rel, Zone &z, bool dragging);
    bool _doDrag(GamePoint src, GamePoint target, Zone &t);
    bool _tileWalkable(GamePoint p, Zone &z);
    bool _tileDraggable(GamePoint p, Zone &t);
public:
    uint16_t charID;
    GamePoint location;

    float direction;
    unsigned actionFrames;

    bool visible = true;

    bool walking   = false;
    bool attacking = false;

    int health;
    int getHealth();
    void addHealth(int health);

    vector<Tile*> inventory;
};
#endif /* defined(__Desktop_Adventure__Hero__) */
