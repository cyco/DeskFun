//
//  Hero.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Hero.hpp"
#include "OpenGL.h"
#include "Tile.hpp"
#include "Renderer.hpp"
Hero::Hero()
: location((GamePoint){.x=4, .y=4, .l=1}), direction(DirectionSouth)
{}

void Hero::face(float dir)
{
    this->direction = dir;
}

bool Hero::move(GamePoint relative, Zone &zone)
{
    GamePoint targetPoint = GamePointAdd(relative, this->location);

    // Look where we're going
    //    this->face(relative);

    // check if target is within the current zone
    const Sizei zoneSize = zone.getSize();
    if(!GamePointInBounds(targetPoint, zoneSize.width, zoneSize.height)) {
        return this->walking = false;
    }

    if(this->_doMove(relative, zone, GamePointIsUnidirectionalMovement(relative) && this->isDragging))
        return this->walking = true;

    int y = relative.y;
    int x = relative.x;

    // try moving horizontally only
    relative.y = 0;
    if(this->_doMove(relative, zone, this->isDragging))
        return this->walking = true;

    // try moving vertically only
    relative.y = y;
    relative.x = 0;
    if(this->_doMove(relative, zone, this->isDragging))
        return this->walking = true;

    // restore original motion
    relative.x = x;
    relative.y = y;

    // don't move diagonally if the hero tries to drag or push something
    if(this->isDragging)
        return this->walking = false;

    // try to avoid the object by moving diagonally
    if(y != 0 && x == 0)
    {
        relative.x = x -1.0;
        if(GamePointInBounds(GamePointAdd(targetPoint, relative), zoneSize.width, zoneSize.height)
           && this->_doMove(relative, zone, false))
            return this->walking = true;

        relative.x = x +1.0;
        if(GamePointInBounds(GamePointAdd(targetPoint, relative), zoneSize.width, zoneSize.height)
           && this->_doMove(relative, zone, false))
            return this->walking = true;
    } else if(x != 0 && y == 0) {
        relative.y = y +1.0;
        if(GamePointInBounds(GamePointAdd(targetPoint, relative), zoneSize.width, zoneSize.height)
           && this->_doMove(relative, zone, false))
            return this->walking = true;

        relative.y = y -1.0;
        if(GamePointInBounds(GamePointAdd(targetPoint, relative), zoneSize.width, zoneSize.height)
           && this->_doMove(relative, zone, false))
            return this->walking = true;
    }

    return this->walking = false;
}

bool Hero::_doMove(GamePoint rel, Zone &z, bool dragging)
{
    if(GamePointsAreEqual(rel, GamePointZero))
        return false;

    GamePoint source = this->location;
    GamePoint target = GamePointAdd(source, rel);

    if(_tileWalkable(target, z))
    {
        if(dragging)
            _doDrag(GamePointSubtract(source, rel), source, z);

        this->location = target;
        return true;

    } else if(dragging) {
        GamePoint dragTarget = GamePointAdd(target, rel);
        if(GamePointInBounds(dragTarget, z.getSize().width, z.getSize().height)
           && _doDrag(target, dragTarget, z))
        {
            this->location = target;
            return true;
        }
    }

    return false;
}

bool Hero::_doDrag(GamePoint src, GamePoint target, Zone &z)
{
    Tile *t = z.getTile(src);
    if(!t || !t->isDraggable())
        return false;

    if(z.getTile(GamePointOnLayer(target, 1)) != NULL)
        return false;

    z.setTile(NULL, src);
    z.setTile(t, target);

    return true;
}

#pragma mark - Helpers
bool Hero::_tileWalkable(GamePoint point, Zone &zone)
{
    // TODO: convert to zone.tileWalkableAt(point);
    GamePoint floorPoint  = GamePointOnLayer(point, 0);
    GamePoint objectPoint = GamePointOnLayer(point, 1);
    GamePoint roofPoint   = GamePointOnLayer(point, 2);

    Tile *t;
    t = zone.getTile(floorPoint);
    if(t != NULL && !t->isWalkable()) return false;

    t = zone.getTile(objectPoint);
    if(t != NULL) return false;

    t = zone.getTile(roofPoint);
    if(t != NULL && !t->isWalkable()) return false;

    return true;
}

#pragma mark -
void Hero::tick(unsigned int ticks)
{
    if(this->walking || this->attacking)
        actionFrames += ticks;
    else actionFrames = 0;
}

void Hero::render(Pointf offset, Renderer *renderer)
{
    if(!this->visible) return;

    if(appearance)
    {
        Tile *t = appearance->getFace(this->direction, actionFrames);
        if(t) renderer->renderTile(t->getTexture(0), offset.x+this->location.x, offset.y+this->location.y, 2);
    }
}

#pragma mark -
bool Hero::attack()
{
    return false;
}

bool Hero::canAttack()
{
    return !this->attacking && ! this->walking;
}

int Hero::getHealth()
{
    return  health;
}

void Hero::addHealth(int h)
{
    health = MIN(health+h, Hero::MAX_HEALTH);
    // TODO: execute change health callback if it's set
}

void Hero::addItem(Tile *item)
{
    inventory.push_back(item);
}

void Hero::removeItem(Tile *item)
{
    for(auto it=inventory.begin(); it < inventory.end(); it++)
    {
        if(*it == item)
        {
            inventory.erase(it);
            return;
        }
    }
}

bool Hero::hasItem(Tile *item)
{
    for(auto it=inventory.begin(); it < inventory.end(); it++)
        if(*it == item)
            return true;
    
    return false;
}