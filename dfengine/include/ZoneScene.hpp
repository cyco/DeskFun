//
//  ZoneScene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__ZoneScene__
#define __Desktop_Adventure__ZoneScene__

#include <stdio.h>
#include "Scene.hpp"

#include "EngineTypes.hpp"
#include "Camera.hpp"
#include "Action.hpp"

class Tile;
class Zone;
class Action;
class ZoneScene : public Scene
{
public:
    ZoneScene(void);

    void init(void) override;
    void prepareCamera(void);
    void cleanup(void) override;

    bool update(double delta) override;
    bool render(double delta) override;

    void setZone(Zone* zone);
    Zone *getZone(void);

    void setEngine(Engine *engine) override;

    Pointf getCurrentOffset();
private:
    unsigned short _ticks;

    Camera _camera;
    Zone *_zone;
    Action *_action;

    void _parseInput(void);
    bool _handleSpecialKeys(void);
    bool _handleKeyboard(void);
    bool _handleMouse(void);

    void _moveHero(float direction);
    void _transition(GamePoint p);
    void _updateLocation(void);
    
    void renderZone(Pointf offset);
#pragma mark -
    unsigned int _random;
    bool executeAction(Action *action);
    unsigned short instructionPointer = 0, locationChanged=false;
    bool evaulateCondition(Action::Instruction condition);
    bool executeInstruction(Action::Instruction instruction);
};
#endif
