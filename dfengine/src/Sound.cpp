//
//  Sound.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Sound.hpp"
#include "Packing.h"

Sound::Sound(FILE* file)
{
    uint16_t length;
    fread(&length, sizeof(length), 1, file);
    
    char fileName[length];
    fread(fileName, length, 1, file);
    fileName[length-1] = '\0';

    this->fileName = string(fileName);
}

size_t Sound::write(char* buffer)
{
    const uint16_t length = fileName.length() + 1;
    size_t bytesWritten = 0;

    // write string length
    if(buffer) uint16_t_pack(buffer+bytesWritten, length);
    bytesWritten += sizeof(length);

    // write name
    if(buffer) memcpy(buffer+bytesWritten, fileName.c_str(), length);
    bytesWritten += length;

    return bytesWritten;
}

string& Sound::getFileName()
{
    return fileName;
}
