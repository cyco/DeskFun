//
//  DFEngine.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__DFEngine__
#define __Desktop_Adventure__DFEngine__

#include <stdio.h>
#include <vector>

#include "GameDataParser.hpp"
#include "SoundMixer.hpp"
#include "InputManager.hpp"
#include "World.hpp"
#include "Renderer.hpp"
#include "GameState.hpp"

using namespace std;

class Scene;


class Engine {
public:
    void init(void);
    void cleanup(void);

    void update(double delta);
    void render(double delta);

    void swapScene(Scene* scene);
    void pushScene(Scene* scene);
    Scene* popScene(void);
    vector<Scene*>& getSceneStack(void);

    Renderer *renderer;
    SoundMixer *mixer;
    InputManager *InputManager;

    // DeskFun specific
    GameDataParser *data;
    World    *world;
    unsigned int ticks;
    struct {
        void *host = NULL;
        void (*setCursor)(float,void*) = NULL;
        void (*updateLocation)(unsigned short, void*) = NULL;
        void (*updateInventory)(void*) = NULL;
        void (*changeAmmo)(unsigned short, void*) = NULL;
        void (*changeHealth)(unsigned short, void*) = NULL;
    } hostCallbacks;

    GameState state;
private:
    vector<Scene*> states;
};

#endif