//
//  GameState.hpp
//  DeskFun Engine
//

//
//

#ifndef GameState_cpp
#define GameState_cpp

#include <stdio.h>

class GameState
{

public:
    size_t getGamesWon();
    size_t getGamesLost();

    size_t gamesWon;
    size_t gamesLost;
};

#endif /* GameState_cpp */
