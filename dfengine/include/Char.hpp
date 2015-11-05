//
//  Char.h
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DAParser__Char__
#define __DAParser__Char__

#include <stdio.h>
#ifdef __cplusplus
#include <string>
#include <array>
#endif

#include "GameObject.hpp"

#define packed __attribute__ ((__packed__))
typedef struct packed {
    uint16_t up;
    uint16_t down;
    uint16_t extension_up;
    uint16_t left;
    uint16_t extension_down;
    uint16_t extension_left;
    uint16_t right;
    uint16_t extension_right; // aka icon in first frame
} CharFrame;

typedef enum
{
    CharTypeHero = 0x0001,
    CharTypeEnemy = 0x0002,
    CharTypeWeapon = 0x0004,
    CharTpeUnknown,
} CharType;

using namespace std;

class Tile;
class Char : GameObject
{
public:
    static const char CATEGORY[];
    static const char INLINE_MARK[];
    static const char WEAPONS_CATEGORY[];
    static const char AUXILIARY_CATEGORY[];
    static const short FRAME_COUNT = 3;

    static vector<Char*> ReadFromFile(FILE *file, range range);
    static void ReadWeaponsFromFile(FILE *file, range range, vector<Char*> chars);
    static void ReadAuxiliariesFromFile(FILE *file, range range, vector<Char*> chars);

    Char(FILE *f);
    ~Char();

    string& getName();
    CharType getType();

    size_t write(char *buffer);
    void readWeapon(FILE *f);
    size_t writeWeapon(char* buffer);
    void readAuxiliary(FILE *f);
    size_t writeAuxiliary(char* buffer);

    Tile* getFace(float direction, unsigned short frame);

private:
    string  name;
    uint8_t weaponData[4];
    uint8_t auxiliaryData[2];

    uint8_t* specs;
    size_t   specsLength;

public:
    CharFrame frames[FRAME_COUNT];

    // debug
    uint8_t *getWeapon(){
        return weaponData;
    }

    uint8_t * getAux(){
        return auxiliaryData;
    }

    size_t getSpecsLength(){
        return specsLength;
    }

    uint8_t* getSpecs(){
        return specs;
    }
};

#endif
