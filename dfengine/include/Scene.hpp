//
//  Scene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__Scene__
#define __Desktop_Adventure__Scene__

#include <stdio.h>

class Engine;

class Scene {
public:
    virtual void init(void) = 0;
    virtual void cleanup(void) = 0;

    virtual bool update(double delta) = 0;
    virtual bool render(double delta) = 0;

    virtual void setEngine(Engine* engine);
    Engine *getEngine();

protected:
    Engine *_engine;
};

#endif