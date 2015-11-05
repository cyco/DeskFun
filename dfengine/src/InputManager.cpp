//
//  InputManager.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "InputManager.hpp"
#include "Direction.hpp"

#pragma mark - Mouse Handling
Pointf InputManager::getMouseLocation(void)
{
    return (Pointf){.x=NAN, .y=NAN};
}

bool InputManager::getLeftMouseDown(void)
{
    return false;
}

bool InputManager::getRightMouseDown(void)
{
    return false;
}

#pragma mark - Key Handling
KeyStates InputManager::getKeyStates(void)
{
    return 0;
}

KeyStates InputManager::getKeyStateChanges(void)
{
    return 0;
}

void InputManager::clearKeyChanges(void)
{}
