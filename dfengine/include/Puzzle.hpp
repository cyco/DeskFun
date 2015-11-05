//
//  Puzzle.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__Puzzle__
#define __Desktop_Adventure__Puzzle__

#include <stdio.h>
#include <array>
#include <string>

#include "GameObject.hpp"

using namespace std;

typedef enum : uint32_t {
    PuzzleTypeU1 = 0,
    PuzzleTypeU2 = 1,
    PuzzleTypeU3 = 2,
    PuzzleTypeEnd = 3,
    PuzzleTypeU4 = 4
} _PuzzleType;
typedef uint32_t PuzzleType;

class Tile;
class Puzzle : GameObject
{
public:
    const static char CATEGORY[];
    const static char NAMES_CATEGORY[];
    const static char INLINE_MARK[];

    static vector<Puzzle*> ReadFromFile(FILE* f, range range);
    static void ReadNamesFromFile(FILE *file, range range, vector<Puzzle*> &puzzles);

    Puzzle(FILE *file);
    ~Puzzle();

    size_t write(char *buffer);
    void readName(FILE *file);
    size_t writeName(char *buffer);

    string& getName();
    PuzzleType getType();
    Tile* getQuestItem(unsigned short index);
    string& getText(unsigned short index);
    
private:
    string name;
    PuzzleType type;
    Tile *questItems[2];

    array<string, 5> text;

public:
    uint16_t questItemIDs[2];
    char* _unknown = NULL;
};

#endif /* defined(__Desktop_Adventure__Puzzle__) */
