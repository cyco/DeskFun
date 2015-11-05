//
//  ZoneStore.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DeskFun_Engine__ZoneStore__
#define __DeskFun_Engine__ZoneStore__

#include <stdio.h>
#include <vector>
#include "Store.hpp"

using namespace std;

class Zone;
class ZoneStore : public std::vector<Zone*>, public Store
{
public:
    // Serialization & Deserialization
    const static char HEADER[];

    ZoneStore();
    ~ZoneStore();

    size_t readFromFile(FILE *file);
    size_t write(char* buffer);
};

#endif /* defined(__DeskFun_Engine__ZoneStore__) */
