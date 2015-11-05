//
//  Puzzle.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Puzzle.hpp"
#include <string>
#include <array>

#include "Packing.h"

#include "Tile.hpp"
#include "GameContext.hpp"
using namespace std;

const char Puzzle::CATEGORY[] = "PUZ2";
const char Puzzle::NAMES_CATEGORY[] = "PNAM";
const char Puzzle::INLINE_MARK[] = "IPUZ";

vector<Puzzle*> Puzzle::ReadFromFile(FILE *file, range range)
{
    uint16_t index;
    vector<Puzzle*> puzzles;
    fseek(file, range.first, SEEK_SET);

    while(fread(&index, sizeof(index), 1, file) && index != 0xFFFF)
        puzzles.push_back(new Puzzle(file));

    return puzzles;
}

void Puzzle::ReadNamesFromFile(FILE *file, range range, vector<Puzzle*> &puzzles)
{
    fseek(file, range.first, SEEK_SET);

    uint16_t numberOfEntries = 0;
    fread(&numberOfEntries, sizeof(uint16_t), 1, file);

    uint8_t n = numberOfEntries;
    while (n--) {
        puzzles[numberOfEntries-n-1]->readName(file);
    }
}


Puzzle::Puzzle(FILE *file)
{
    char marker[4];
    fread(marker, sizeof(char), 4, file);
    if(memcmp(Puzzle::INLINE_MARK, marker, strlen(INLINE_MARK)) != 0)
    {
        printf("Unable to read inline puzzle header %s. The data file is probably corrupted.\n", Puzzle::INLINE_MARK);
        return;
    }

    uint32_t size;
    fread(&size, sizeof(size), 1, file);

    if(context->getGameStyle() == GameStyleYoda)
        fread(&type, sizeof(type), 1, file);

    _unknown = new char[15]();
    fread(_unknown, 1, 10, file);

    for(int i=0; i < 5; i++)
    {
        uint16_t length;
        fread(&length, sizeof(length), 1, file);

        char * cstring = new char[length+1]();
        fread(cstring, length, 1, file);
        *(cstring+length) = '\0';
        text[i] = string(cstring);
        delete [] cstring;
    }

    uint16_t itemIDs[2] = { 0 };

    if(context->getGameStyle() == GameStyleYoda)
        fread(itemIDs, sizeof(uint16_t), 2, file);
    else
    {
        fread(itemIDs, sizeof(uint16_t), 1, file);
        itemIDs[1] = 0xFFFF;
    }

    questItemIDs[0] = itemIDs[0];
    questItemIDs[1] = itemIDs[1];

    /*
     if(questItemIDs[0] == 0x63c)
     {
     printf("Unknown: ");
     for(int j=0; j < 10; j++)
     {
     printf("%02x ", _unknown[j]);
     }
     printf("\n");
     }
     */

    questItems[0] = context->getTile(itemIDs[0]) ?: context->getTile(0);
    questItems[1] = context->getTile(itemIDs[1]) ?: context->getTile(0);
}

Puzzle::~Puzzle()
{
    if(_unknown)  delete [] _unknown;
}

#pragma mark - Serialization
size_t Puzzle::write(char *buffer)
{
    const bool isYoda = context->getGameStyle()==GameStyleYoda;
    size_t bytesWritten = 0;

    // identifier
    if(buffer) memcpy(buffer+bytesWritten, Puzzle::INLINE_MARK, 4);
    bytesWritten += 4;

    // size
    int string_lengths = 0;
    for(int j=0; j < 5; j++) string_lengths += text[j].length();

    uint32_t data_size = (isYoda ? 28 : 22) + string_lengths;
    if(buffer) uint32_t_pack(buffer+bytesWritten, data_size);
    bytesWritten += sizeof(uint32_t);

    // puzzle type
    if(isYoda)
    {
        if(buffer) uint32_t_pack(buffer+bytesWritten, this->getType());
        bytesWritten += sizeof(uint32_t);
    }

    if(buffer) memcpy(buffer+bytesWritten, this->_unknown, 10);
    bytesWritten += 10;

    // encode puzzle strings
    for(int j=0; j < 5; j++){
        std::string string = text[j];

        // string length
        if(buffer) uint16_t_pack(buffer+bytesWritten, string.length());
        bytesWritten += sizeof(uint16_t);

        // text
        if(buffer) sprintf(buffer+bytesWritten, "%s", string.c_str());
        bytesWritten += string.length();
    }

    // encode related quest item
    if(buffer) uint16_t_pack(buffer+bytesWritten, questItemIDs[0]);
    bytesWritten += sizeof(uint16_t);

    // yoda has another quest item
    if(isYoda)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, questItemIDs[1]);
        bytesWritten += sizeof(uint16_t);
    }

    return bytesWritten;
}

void Puzzle::readName(FILE *file)
{
    char name[0x10] = {0};
    fread(name, sizeof(char), 0x10, file);
    this->name = string(name);
}

size_t Puzzle::writeName(char *buffer)
{
    return 0;
}

#pragma mark -
string& Puzzle::getName()
{
    return name;
}

PuzzleType Puzzle::getType()
{
    return type;
}

Tile* Puzzle::getQuestItem(unsigned short index)
{
    return questItems[index];
}

string& Puzzle::getText(unsigned short index)
{
    return text[index];
}
