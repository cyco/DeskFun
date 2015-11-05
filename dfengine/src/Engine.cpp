//
//  DFEngine.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Engine.hpp"

#include "Scene.hpp"

void Engine::init(void)
{
    for(int i=0; i < data->_tiles.size(); i++)
    {
        data->_tiles[i]->setupTextures(this->renderer);
    }
}

void Engine::cleanup(void)
{}

#pragma mark -
void Engine::update(double delta)
{
    ticks += delta;

    if(states.size())
        states[states.size()-1]->update(delta);
}

void Engine::render(double delta)
{
    vector<Scene*> currentStates;
    copy(states.begin(), states.end(), back_inserter(currentStates));

    // for (int i=(int)currentStates.size()-1; i >= 0; i--) {
    for (int i=0; i < currentStates.size(); i++) {
        Scene *s = currentStates[i];
        if(s != NULL) s->render(delta);
    }
}

#pragma mark - state managment
void Engine::swapScene(Scene* scene)
{
    this->popScene();
    this->pushScene(scene);
}

void Engine::pushScene(Scene* scene)
{
    printf("Engine::pushScene()\n");
    if(InputManager) InputManager->clearKeyChanges();

    scene->setEngine(this);
    scene->init();
    scene->update(0);
    states.push_back(scene);
}

Scene* Engine::popScene(void)
{
    printf("Engine::popScene()\n");
    if(InputManager) InputManager->clearKeyChanges();

    if(states.size() == 1)
    {
        printf("Prevented popping last scene! This is a programming error!\n");
        return NULL;
    }

    Scene *scene = states[states.size()-1];
    scene->cleanup();
    states.pop_back();

    return scene;
}

vector<Scene*>& Engine::getSceneStack(void)
{
    return states;
}