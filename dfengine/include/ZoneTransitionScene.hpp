//
//  ZoneTransitionScene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__ZoneTransitionScene__
#define __Desktop_Adventure__ZoneTransitionScene__

#include <stdio.h>

#include "Scene.hpp"
#include "EngineTypes.hpp"

// TODO: make ZoneTransitionScene abstract class called TransitionScene
// TODO: move _renderZoneAnimation to subclass ZoneTransitionScene
// TODO: move _renderRoomAnimation to subclass RoomTransitionScene

class Tile;
class Zone;
class ZoneScene;
class ZoneTransitionScene : public Scene
{
public:
    typedef enum {
        RoomTransition,
        ZoneTransition,
    } TransitionType;

    ZoneTransitionScene(TransitionType type, ZoneScene *scene);

    void init(void) override;
    void cleanup(void) override;

    bool update(double delta) override;
    bool render(double delta) override;

    Zone *oldZone;
    Zone *newZone;

    GamePoint heroStart, heroEnd;
    GamePoint zoneStart, zoneEnd;

    bool snapAnimationToTiles;
private:
    ZoneScene *_scene;
    TransitionType _type;

    float duration;
    float zoneSwapTime;
    float state;

    void _renderZoneAnimation();
    void _renderRoomAnimation();
};

#endif