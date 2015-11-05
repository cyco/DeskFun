//
//  GameObject.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "GameObject.hpp"

#include "GameContext.hpp"
const unsigned short GameObject::CATEGORY_LENGTH = 4;
const unsigned short GameObject::MARKER_LENGTH = 4;

GameObject::GameObject()
{
    context = GameContext::CurrentContext();
}
