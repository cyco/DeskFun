//
//  TileStore.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "TileStore.hpp"
#include "Tile.hpp"
#include "Packing.h"
#include "GameContext.hpp"
const char TileStore::HEADER[] = "TILE";

TileStore::TileStore() : vector<Tile*>(0)
{}

#pragma mark - Serialization
size_t TileStore::readFromFile(FILE *file)
{
    this->readPastHeader(HEADER, file);
    size_t categorySize = this->readSize(file);
    size_t tileCount = categorySize / (4 + Tile::SIZE);

    vector<Tile*> tiles = vector<Tile*>(tileCount);
    for(unsigned int i=0; i < tileCount; i++)
    {
        Tile *tile = new Tile(file);
        this->push_back(tile);
        tiles[i] = tile;
        tiles[i]->index = i;

         GameContext::CurrentContext()->tileStore.RegisterTile(tiles[i]);
    }
    return tileCount;
}

size_t TileStore::write(void* buffer)
{
    size_t bytesWritten = 0;
    size_t tileCount = this->size();
    size_t headerSize = HEADER_LENGTH + sizeof(uint32_t);
    size_t contentSize = tileCount * (4 + Tile::SIZE);

    if(!buffer)
        return headerSize + contentSize;

    // write header
    memcpy((char*)buffer+bytesWritten, HEADER, HEADER_LENGTH);
    bytesWritten += GameObject::CATEGORY_LENGTH;

    // write content size
    uint32_t_pack((char*)buffer+bytesWritten, (uint32_t)contentSize);
    bytesWritten += sizeof(uint32_t);

    for(int i=0; i < tileCount; i++){
        bytesWritten += this->at(i)->write((unsigned char*)buffer+bytesWritten);
    }
    
    return bytesWritten;
}

TileStore::~TileStore()
{
    for(int i=0; i < this->size(); i++)
        delete this->at(i);
}

unsigned int TileStore::getTileCount()
{
    return (unsigned int)this->size();
}

Tile* TileStore::GetTile(unsigned id)
{
    if(id >= this->size()) return NULL;
    return this->at(id);
}

unsigned TileStore::RegisterTile(Tile* t)
{
    this->push_back(t);
    return (unsigned) this->size() -1;
}

void TileStore::UnregisterTile(Tile* t)
{
    printf("TileStore::UnregisterTile is not implemented yet.\n", stderr);
}
