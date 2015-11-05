//
//  SpeechScene.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__SpeechScene__
#define __Desktop_Adventure__SpeechScene__

#include <stdio.h>
#include "Scene.hpp"
#include "EngineTypes.hpp"

class SpeechScene : public Scene
{
public:
    void init(void) override;
    void cleanup(void) override;

    bool update(double delta) override;
    bool render(double delta) override;
};
#endif
