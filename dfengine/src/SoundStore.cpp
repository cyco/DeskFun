//
//  SoundStore.cpp
//  DeskFun Engine
//

//
//

#include "SoundStore.hpp"

#include "Sound.hpp"
#include "Packing.h"

const char SoundStore::HEADER[] = "SNDS";

SoundStore::SoundStore() : std::vector<Sound*>(0)
{}

#pragma mark - Serialization
size_t SoundStore::readFromFile(FILE *file)
{
    this->readPastHeader(HEADER, file);
    this->readSize(file);

    int16_t item_count;
    fread(&item_count, sizeof(uint16_t), 1, file);
    item_count *= -1;

    for(int i=0; i < item_count; i++)
        this->push_back(new Sound(file));

    return item_count;
}

size_t SoundStore::write(void* buffer)
{
    const uint16_t soundCount = this->size();
    size_t bytesWritten = 0;

    // write category header
    if(buffer) memcpy(buffer, HEADER, HEADER_LENGTH);
    bytesWritten += HEADER_LENGTH;

    // get content size
    size_t contentSize = sizeof(uint16_t); // number of sounds
    for(size_t i=0; i < soundCount; i++)
        contentSize += this->at(i)->write(NULL);

    // write content size
    if(buffer) uint32_t_pack((char*)buffer+bytesWritten, (uint32_t)(contentSize));
    bytesWritten += sizeof(uint32_t);

    // write number of sounds (*-1)
    if(buffer) uint16_t_pack((char*)buffer+bytesWritten, (uint16_t)(soundCount*-1));
    bytesWritten += sizeof(uint16_t);

    for(size_t i=0; i < soundCount; i++)
        bytesWritten += this->at(i)->write(buffer ? (char*)buffer+bytesWritten : NULL);

    return bytesWritten;
}

SoundStore::~SoundStore()
{
    for(int i=0; i < this->size(); i++)
        delete this->at(i);
}
