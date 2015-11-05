//
//  ZoneTransitionScene.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "ZoneTransitionScene.hpp"
#include "Engine.hpp"

#include "Hero.hpp"
#include "World.hpp"
#include "ZoneScene.hpp"
#include "Tile.hpp"

#include "OpenGL.h"

#if !defined(MIN)
#define MIN(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
#define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

const static float FramesPerAnimationStep = 6.0f;

ZoneTransitionScene::ZoneTransitionScene(TransitionType type, ZoneScene *scene)
    : _scene(scene), _type(type)
{
    switch (type) {
        case RoomTransition:
            duration = 10 * FramesPerAnimationStep;
            zoneSwapTime = duration / 2.0;
            snapAnimationToTiles = true;
            break;
        case ZoneTransition:
            duration = 9.0 * FramesPerAnimationStep;
            zoneSwapTime = duration;
            snapAnimationToTiles = false;
            break;

        default:
            break;
    }
}

void ZoneTransitionScene::init(void)
{}

void ZoneTransitionScene::cleanup(void)
{}

bool ZoneTransitionScene::render(double delta)
{
    state += delta * 60.0f;

    switch (_type)
    {
        case ZoneTransition: _renderZoneAnimation();
            break;
        case RoomTransition: _renderRoomAnimation();
            break;

        default: break;
    }

    return false;
}

void ZoneTransitionScene::_renderZoneAnimation(void)
{
    const GLfloat w = 9.0, h = 9.0; // viewport size

    const GLfloat newZoneWidth  = newZone->getSize().width;
    const GLfloat newZoneHeight = newZone->getSize().height;

    const GLfloat xDir = zoneStart.x - zoneEnd.x;
    const GLfloat yDir = zoneStart.y - zoneEnd.y;

    const Pointf startingOffset = _scene->getCurrentOffset();
    const GLfloat animationFactor = MIN(state / duration, 1.0);

    const float xOffset = xDir * w * animationFactor + startingOffset.x;
    const float yOffset = yDir * h * animationFactor + startingOffset.y;

    const GLfloat newZoneX = xOffset - xDir * newZoneWidth;
    const GLfloat newZoneY = yOffset - yDir * newZoneWidth;

    const GLfloat oldZoneX = xOffset;
    const GLfloat oldZoneY = yOffset;

    for(float l=0; l < 3; l++){
        for(float y=0; y < newZoneHeight; y++){
            for(float x=0; x < newZoneWidth; x++)
            {
                Tile *t = newZone->getTile(x, y, l);
                if(t) _engine->renderer->renderTile(t->getTexture(0), x+newZoneX, y+newZoneY, l);
                t = oldZone->getTile(x, y, l);
                if(t) _engine->renderer->renderTile(t->getTexture(0), x+oldZoneX, y+oldZoneY, l);
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        if(l == 1)
        {
            _engine->world->hero.render((Pointf){.x=oldZoneX,.y=oldZoneY}, _engine->renderer);
        }
    }

//    GLfloat offsetY = 0.0
}

void ZoneTransitionScene::_renderRoomAnimation(void)
{
    bool fadeIn = state > duration / 2.0;
    double adjustedState = state - (fadeIn ? duration / 2.0 : 0.0);

    GLfloat t; // number of black tiles

    t = adjustedState / FramesPerAnimationStep;
    if(fadeIn) t = 6.0 - t;
    if(snapAnimationToTiles)
        t = floor(t);

    const GLfloat w = 9.0; // viewport width
    const GLfloat h = 9.0; // viewport height
#ifdef glColor4f
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
 //    glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_QUADS); // bottom
    glVertex2f(0.0f, 0.0f);
    glVertex2f(   w, 0.0f);
    glVertex2f(   w,    t);
    glVertex2f(0.0f,    t);
    glEnd();

    glBegin(GL_QUADS); // top
    glVertex2f(0.0f,     h);
    glVertex2f(   w,     h);
    glVertex2f(   w, h - t);
    glVertex2f(0.0f, h - t);
    glEnd();

    glBegin(GL_QUADS); // Left
    glVertex2f(0.0f, 0.0f);
    glVertex2f(   t, 0.0f);
    glVertex2f(   t,    h);
    glVertex2f( 0.0,    h);
    glEnd();

    glBegin(GL_QUADS); // Right
    glVertex2f(  w, 0.0f);
    glVertex2f(w-t, 0.0f);
    glVertex2f(w-t,    h);
    glVertex2f(  w,    h);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0,1.0, 1.0);
#endif

}

bool ZoneTransitionScene::update(double delta)
{
    if(!isinf(zoneSwapTime) && state > zoneSwapTime)
    {
        World *world = _engine->world;
        Hero &hero = world->hero;

        hero.location = heroEnd;
        world->zoneLocation = zoneEnd;

        _scene->setZone(newZone);
        _scene->prepareCamera();

        hero.visible = true;

        // make sure we don't swap again
        zoneSwapTime = INFINITY;
    }

    if(state >= duration)
    {
        _engine->popScene();
    }

    return false;
}

