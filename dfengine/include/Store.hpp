//
//  Store.hpp
//  DeskFun Engine
//

//
//

#ifndef Store_cpp
#define Store_cpp

#include <stdio.h>
#include <vector>

class Store
{
public:
    const static size_t HEADER_LENGTH = 4;

protected:

    void readPastHeader(const char* header, FILE *file);
    size_t readSize(FILE *file);
};

#endif /* Store_cpp */
