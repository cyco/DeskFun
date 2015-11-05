//
//  Scene.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Scene.hpp"

void Scene::setEngine(Engine* engine)
{
    _engine = engine;
}

Engine* Scene::getEngine()
{
    return _engine;
}
