//
//  Camera.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include <stdio.h>
#include <math.h>

#include "Camera.hpp"
#include "Engine.hpp"
#include "ZoneScene.hpp"
#include "World.hpp"

#if !defined(MIN)
#define MIN(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
#define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

Camera::Camera(ZoneScene *scene)
    : _size({.width = 9, .height = 9}), _scene(scene)
{
    _offset.x = _offset.y = 0.0;
}

void Camera::setEngine(Engine *engine)
{
    _engine = engine;
}

void Camera::update(double delta)
{
    if(!_engine) return;

    Zone *zone  = _scene->getZone();
    if(!zone) return;

    World *world = _engine->world;
    GamePoint l = world->hero.location;

    // center
    _offset.x = (_size.width  / 2.0) - l.x;
    _offset.y = (_size.height / 2.0) - l.y;

    // be digital
    _offset.x = floorf(_offset.x);
    _offset.y = floorf(_offset.y);

    // make sure not to scroll over the border
    // left, bottom
    _offset.x = MIN(0, _offset.x);
    _offset.y = MIN(0, _offset.y);
    
    // top, right
    _offset.x = MAX(_offset.x, _size.width -zone->getSize().width);
    _offset.y = MAX(_offset.y, _size.height-zone->getSize().height);
}

Pointf Camera::getOffset()
{
    return _offset;
}

Sizef Camera::getSize()
{
    return _size;
}
