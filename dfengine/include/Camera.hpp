//
//  Camera.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef Desktop_Adventure_Camera_hpp
#define Desktop_Adventure_Camera_hpp
#include "EngineTypes.hpp"

class ZoneScene;
class Engine;
class Camera
{
public:
    Camera(ZoneScene *scene);
    
    void update(double delta);

    Pointf getOffset();
    Sizef  getSize();

    void setEngine(Engine *engine);
private:
    Pointf _offset;
    Sizef  _size;
    ZoneScene *_scene;
    Engine *_engine;
};
#endif
