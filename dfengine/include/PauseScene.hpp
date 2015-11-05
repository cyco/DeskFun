//
//  PauseScene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__PauseScene__
#define __Desktop_Adventure__PauseScene__

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>

#include "OpenGL.h"
#include "Scene.hpp"

class PauseScene : public Scene
{
public:
    void init(void) override;
    void cleanup(void) override;

    bool update(double delta) override;
    bool render(double delta) override;

private:
    GLuint texture;
    GLuint LoadTexture(CFStringRef name);
};
#endif
