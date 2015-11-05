//
//  Char.cpp
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Char.hpp"
#include "Packing.h"
#include "Hero.hpp"
#include "Direction.hpp"

#include "GameContext.hpp"

const char Char::CATEGORY[]           = "CHAR";
const char Char::WEAPONS_CATEGORY[]   = "CHWP";
const char Char::AUXILIARY_CATEGORY[] = "CAUX";
const char Char::INLINE_MARK[]        = "ICHA";

vector<Char*> Char::ReadFromFile(FILE *file, range range)
{
    fseek(file, range.first, SEEK_SET);

    vector<Char*> chars;

    uint16_t index;
    while(fread(&index, sizeof(index), 1, file) && index != 0xFFFF)
    {
        char identifier[4];
        fread(identifier, 4, 1, file); // read ICHA identifier

        Char *chara = new Char(file);
        chars.push_back(chara);
    }

    return chars;
}

void Char::ReadWeaponsFromFile(FILE *file, range range, vector<Char*> chars)
{
    fseek(file, range.first, SEEK_SET);

    uint16_t index;
    while(fread(&index, sizeof(index), 1, file) && index != 0xFFFF)
        chars[index]->readWeapon(file);
}

void Char::ReadAuxiliariesFromFile(FILE *file, range range, vector<Char*> chars)
{
    fseek(file, range.first, SEEK_SET);

    uint16_t index;
    while(fread(&index, sizeof(index), 1, file) && index != 0xFFFF)
        chars[index]->readAuxiliary(file);
}

Char::Char(FILE *f)
{
    uint32_t size;
    fread(&size, sizeof(size), 1, f);

    int frame_size = sizeof(CharFrame);
    uint8_t *data = new uint8_t[size-3*frame_size];
    fread(data, size-3*frame_size, 1, f);

    // data contains character info as follows (differs for indy and yoda)
    // 11 bytes unknown padding, 2 bytes type (0x0001 Hero, 0x0002 Enemey, 0x0004 Weapon)
    // 2 bytes unknown
    // then for yoda it's 6 additional bytes unknown which are always 00 FF FF 00 00 00 except for Lightsaber & Boba Fett
    size_t name_lenth = strlen((char*)data) +1;
    char *name = new char[name_lenth];
    memcpy(name, data, name_lenth);

    // zero out name
    memset(data, 0, name_lenth);

    // as well as three frames character animation, each 8 times 2 byte tile reference
    // layed out like this
    // up, down, extension_up, left, extension_down, extension_left, right, extension_right
    // extension_right in first frame is sometimes reused as inv-icon
    CharFrame *frames = new CharFrame[3]();
    fread(frames, frame_size, 3, f);

    this->name = string(name);
    this->specs = data; // fix
    this->specsLength = size-3*frame_size; // fix

    this->frames[0] = frames[0];
    this->frames[1] = frames[1];
    this->frames[2] = frames[2];

    delete [] name;
}

Char::~Char(){
    delete [] specs;
}
#pragma mark -
string& Char::getName(){
    return name;
}

CharType Char::getType(){
    return (CharType)(specs[16] | specs[17] << 8);
}
#pragma mark - Serialization
size_t Char::write(char *buffer)
{
    long entry_size  = (context->getGameStyle() == GameStyleYoda ? 0x54 : 0x4E);

    size_t bytesWritten = 0;
    if(buffer) memcpy(buffer+bytesWritten, Char::INLINE_MARK, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    uint32_t charSize = (uint32_t)entry_size - 2 - 8;
    if(buffer) uint32_t_pack(buffer+bytesWritten, (uint32_t)charSize);
    bytesWritten += sizeof(uint32_t);

    // write name
    size_t name_length = name.length()+1;
    if(buffer) sprintf(buffer+bytesWritten, "%s", name.c_str());
    bytesWritten += name_length;

    // write data
    size_t spec_length = specsLength-name_length;
    if(buffer) memcpy(buffer+bytesWritten, specs+name_length, spec_length);
    bytesWritten += spec_length;

    // write frames
    for(int j=0; j < 3; j++)
    {
        CharFrame frame = frames[j];
        if(buffer) memcpy(buffer+bytesWritten, &frame, sizeof(CharFrame));
        bytesWritten += sizeof(CharFrame);
    }

    if(bytesWritten != entry_size-2)
        printf("Erorr\n");

    return bytesWritten;
}

void Char::readWeapon(FILE *f)
{
    fread(weaponData, sizeof(uint8_t), 4, f);
}

size_t Char::writeWeapon(char *buffer)
{
    if(buffer) memcpy(buffer, (char*)weaponData, 4);
    return 4;
}

void Char::readAuxiliary(FILE *f)
{
    fread(auxiliaryData, sizeof(uint8_t), 2, f);
}

size_t Char::writeAuxiliary(char *buffer)
{
    if(buffer) memcpy(buffer, auxiliaryData, 2);
    return 2;
}

Tile* Char::getFace(float direction, unsigned short frameCount)
{
    if(this->getType() == CharTypeHero)
    {
        uint16_t tileID = 0xFFFF;
        CharFrame frame = frames[frameCount%3];

        int dir = Direction::Confine(direction);
        switch (dir) {
            case Direction::NorthEast:
            case Direction::NorthWest:
            case Direction::North: tileID = frame.up; break;
            case Direction::SouthEast:
            case Direction::SouthWest:
            case Direction::South: tileID = frame.down; break;
            case Direction::East: tileID = frame.right; break;
            case Direction::West: tileID = frame.left; break;
            default:
                break;
        }

        return context->getTile(tileID);
    }

    return NULL;
}

