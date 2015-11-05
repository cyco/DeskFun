//
//  InputManager.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__InputManager__
#define __Desktop_Adventure__InputManager__

#include <stdio.h>
#include "EngineTypes.hpp"

typedef unsigned KeyStates;

class InputManager
{
public:
    typedef enum {
        Drag   = 1<<0,
        Attack = 1<<1,
        Pause  = 1<<2,
        Map    = 1<<3,

        Left   = 1<<4,
        Right  = 1<<5,
        Up     = 1<<6,
        Down   = 1<<7,
    } Key;

    virtual Pointf getMouseLocation(void);
    virtual bool getLeftMouseDown(void);
    virtual bool getRightMouseDown(void);

    virtual KeyStates getKeyStates(void);
    virtual KeyStates getKeyStateChanges(void);
    virtual void clearKeyChanges(void);
};

#endif /* defined(__Desktop_Adventure__InputManager__) */
