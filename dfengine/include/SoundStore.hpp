//
//  SoundStore.hpp
//  DeskFun Engine
//

//
//

#ifndef SoundStore_cpp
#define SoundStore_cpp

#include <stdio.h>
#include "Store.hpp"

class Sound;
class SoundStore : public std::vector<Sound*>, Store
{
public:
    // Serialization & Deserialization
    const static char HEADER[];

    SoundStore();
    ~SoundStore();

    size_t readFromFile(FILE *file);
    size_t write(void* buffer);
};
#endif /* SoundStore_cpp */
