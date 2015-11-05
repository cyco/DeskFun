//
//  Store.cpp
//  DeskFun Engine
//

//
//

#include "Store.hpp"
#include "Packing.h"

void Store::readPastHeader(const char* header, FILE *file)
{
    size_t bytes_read = 0;
    char data[HEADER_LENGTH+1] = {0};

    bytes_read = fread(data, 1, HEADER_LENGTH, file);
    if(bytes_read < HEADER_LENGTH)
    {
        fseek(file, -1 * bytes_read, SEEK_CUR);
        throw std::runtime_error("Not enough data");
    }

    if(memcmp(header, data, HEADER_LENGTH) != 0)
    {
        fseek(file, -1 * bytes_read, SEEK_CUR);
        throw std::runtime_error("Unexpected category identifier");
    }
}

size_t Store::readSize(FILE *file)
{
    uint32_t size = 0;
    size_t items_read = fread(&size, sizeof(size), 1, file);
    from_little_endian(uint32_t, bytes_read);
    if(items_read < 1)
    {
        fseek(file, -1 * items_read, SEEK_CUR);
        throw std::runtime_error("Not enough data");
    }

    return size;
}