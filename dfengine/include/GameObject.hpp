//
//  GameObject.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__GameObject__
#define __Desktop_Adventure__GameObject__

#include <stdio.h>

#ifdef __cplusplus
#include <vector>
#endif

#include "ParserTypes.hpp"
class GameContext;
class GameObject
{
public:
    const static unsigned short CATEGORY_LENGTH;
    const static unsigned short MARKER_LENGTH;

protected:
    GameObject();
    GameContext *context;
};

#endif /* defined(__Desktop_Adventure__GameObject__) */
