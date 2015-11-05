//
//  Sound.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DFSound__
#define __DFSound__

#include <stdio.h>
#include <string>
#include <vector>

#include "ParserTypes.hpp"

using namespace std;


class Sound
{

public: // Serialization
    Sound(FILE* file);
    size_t write(char* buffer);
protected:
    string fileName;


public:
    string& getFileName();

};

#endif