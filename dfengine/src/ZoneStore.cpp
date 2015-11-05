//
//  ZoneStore.cpp
//  DeskFun Engine
//

//
//

#include "ZoneStore.hpp"
#include "Zone.hpp"
#include "Packing.h"
#include "GameContext.hpp"

const char ZoneStore::HEADER[] = "ZONE";

ZoneStore::ZoneStore() : vector<Zone*>(0)
{}

ZoneStore::~ZoneStore()
{
    for(int i=0; i < this->size(); i++)
        delete this->at(i);
}

size_t ZoneStore::readFromFile(FILE *file)
{
    this->readPastHeader(HEADER, file);
    this->readSize(file);

    if(GameContext::CurrentContext()->getGameStyle() == GameStyleYoda)
        // Yoda's file format uses category length as zone count
        fseek(file, -4, SEEK_CUR);

    uint16_t count = 0;
    fread(&count, sizeof(count), 1, file);

    for(int i=0; i < count; i++)
    {
        Zone *zone = new Zone(file);
        zone->index = i;
        this->push_back(zone);
    }

    return count;
}

size_t ZoneStore::write(char* buffer)
{
    const bool isYodaVersion = GameContext::CurrentContext()->getGameStyle() == GameStyleYoda;
    size_t bytesWritten = 0;

    if(buffer) memcpy(buffer+bytesWritten, HEADER, HEADER_LENGTH);
    bytesWritten += HEADER_LENGTH;

    if(isYodaVersion)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, (uint16_t)this->size());
        bytesWritten += sizeof(uint16_t);
    } else {
        // TODO
        uint32_t size = 0;
        if(buffer) uint32_t_pack(buffer+bytesWritten, size);
        bytesWritten += sizeof(uint32_t);
    }

    for(int i=0; i < this->size(); i++){
        bytesWritten += this->at(i)->write(buffer ? buffer+bytesWritten : NULL);
    }

    return bytesWritten;
}
