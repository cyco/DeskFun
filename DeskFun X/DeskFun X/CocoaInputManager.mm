//
//  CocoaInputManager.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "CocoaInputManager.hpp"

#include "DACursor.h"

#pragma mark - Mouse Handling
Pointf CocoaInputManager::getMouseLocation(void)
{
    return _mouseLocation;
}

bool CocoaInputManager::getLeftMouseDown(void)
{
    return _leftMouseDown;
}
bool CocoaInputManager::getRightMouseDown(void)
{
    return _rightMouseDown;
}

#pragma mark - Key Handling
void CocoaInputManager::changeKey(InputManager::Key key, bool state)
{
    bool keyDown  = (_keyStates & key) != 0;
    if(keyDown == state)
        return; // no change

    _keyStates ^= key;
    _keyStateChanges |= key;
}

KeyStates CocoaInputManager::getKeyStates(void)
{
    return _keyStates;
}

KeyStates CocoaInputManager::getKeyStateChanges(void)
{
    return _keyStateChanges;
}

void CocoaInputManager::clearKeyChanges(void)
{
    _keyStateChanges = 0;
}
