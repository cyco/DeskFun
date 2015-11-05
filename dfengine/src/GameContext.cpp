//
//  GameContext.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "GameContext.hpp"
#include "Palette.h"

static GameContext *CurrentGameContext = new GameContext();

GameContext* GameContext::CurrentContext()
{
    return CurrentGameContext;
}

GameContext::GameContext() : style(GameStyleIndy)
{}

GameStyle GameContext::getGameStyle()
{
    return style;
}

const unsigned char* GameContext::getPalette()
{
    return this->getGameStyle() == GameStyleYoda ? yodas_palette : indys_pallette;
}

unsigned int GameContext::getTileCount()
{
    return tileStore.getTileCount();
}

Tile* GameContext::getTile(unsigned int tileID)
{
    return tileStore.GetTile(tileID);
}

void GameContext::switchToGameStyle(GameStyle s)
{
    style = s;
}