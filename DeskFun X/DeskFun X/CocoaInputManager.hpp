//
//  CocoaInputManager.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__CocoaInputManager__
#define __Desktop_Adventure__CocoaInputManager__

#include <stdio.h>
#include "InputManager.hpp"

class CocoaInputManager : public InputManager
{
public:
    Pointf getMouseLocation(void) override;
    bool getLeftMouseDown(void) override;
    bool getRightMouseDown(void) override;

    void changeKey(InputManager::Key key, bool state);
    void clearKeyChanges(void) override;
    KeyStates getKeyStates(void) override;
    KeyStates getKeyStateChanges(void) override;

public:
    bool _leftMouseDown, _rightMouseDown;

    KeyStates _keyStates;
    KeyStates _keyStateChanges;

    Pointf _mouseLocation;
};
#endif /* defined(__Desktop_Adventure__CocoaInputManager__) */
