//
//  Tile.cpp
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Tile.hpp"
#include "GameDataParser.hpp"
#include "OpenGL.h"
#include "Packing.h"
#include "TileStore.hpp"

#pragma mark - Serialization
Tile::Tile(FILE *f) : GameObject()
{
    // read tile specificatoin
    fread(&_specs, sizeof(uint32_t), 1, f);

    // read palette data
    _data = new uint8_t[Tile::SIZE];
    fread(_data, Tile::SIZE, sizeof(uint8_t), f);
}

size_t Tile::write(void* buffer)
{
    size_t bytesWritten = 0;

    if(buffer) uint32_t_pack((char*)buffer, _specs);
    bytesWritten += sizeof(_specs);

    if(buffer) memcpy((char*)buffer+bytesWritten, _data, Tile::SIZE);
    bytesWritten += Tile::SIZE;

    return bytesWritten;
}

Tile::~Tile(void)
{
    delete [] _data;
}

#pragma mark -

const char Tile::NAMES_CATEGORY[] = "TNAM";

typedef enum {
    TransparentBit = 1,
    DraggableBit = 3,
    LocatorBit   = 5
} TileSpecs;



void Tile::ReadNamesFromFile(FILE *file, range range, vector<Tile*> &tiles)
{
    size_t end = range.first + range.second;
    fseek(file, range.first, SEEK_SET);

    uint16_t tileID;
    while(ftell(file) < end && fread(&tileID, sizeof(tileID), 1, file) && tileID != 0xFFFF)
        tiles[tileID]->readName(file);
}

Tile::Tile(void) : GameObject()
{
    _data = new uint8_t[0];
}

Tile::Tile(uint16_t id, uint32_t specs, uint8_t *data)
     : GameObject(), index(id), _data(data), _specs(specs)
{}

#pragma mark - additoinal serialization

void Tile::readName(FILE *f)
{
    size_t entryLength = context->getGameStyle() == GameStyleYoda ? 0x18 : 0x10;

    char *name = new char[entryLength+1];
    std::fread(name, entryLength, 1, f);
    name[entryLength] = '\0';

    this->name = string(name);
    delete [] name;
}

size_t Tile::writeName(void* buffer)
{
    if(!this->name.size()) return 0;

    size_t entryLength = context->getGameStyle() == GameStyleYoda ? 0x18 : 0x10;

    if(buffer) memset(buffer, 0x00, entryLength);
    if(buffer) memcpy(buffer, name.c_str(), name.size());

    return entryLength;
}
#pragma mark - Info
bool Tile::isWalkable() const
{
// TODO: implement
    return true;
}

bool Tile::isDraggable() const
{
    return (_specs & (1 << DraggableBit));
}
bool Tile::isLocatorTile() const
{
    return (_specs & (1 << LocatorBit));
}

uint16_t Tile::getLocatorType() const
{
    /*
    TestBit(17, "home");
    TestBit(18, "puz, unsolved");
    TestBit(19, "puz, solved");
    TestBit(20, "port, unsolved");
    TestBit(21, "port, solved");
    TestBit(22, "N unso");
    TestBit(23, "S unso");
    TestBit(24, "E unso");
    TestBit(25, "W unso");

    TestBit(26, "N solved");
    TestBit(27, "S solved");
    TestBit(28, "E solved");
    TestBit(29, "W solved");

    TestBit(30, "Goal");
     */
    return (_specs & 0x7FFE0000) > 16;
}
#pragma mark - OpenGL
void Tile::setupTextures(Renderer *r)
{
    PalletteAnimation animation = effectivePaletteAnimation();
    unsigned char **pixelData = new unsigned char*[animation.length];

    _textures = (GLuint*)calloc(sizeof(GLuint), animation.length);
    glGenTextures(animation.length, _textures);
    _textureCount = animation.length;

    const unsigned char *palette = this->context->getPalette();

    for(int animID=0; animID < animation.length; animID++)
    {
        pixelData[animID] = new unsigned char[Tile::SIZE * 4];
        unsigned char *animationTile = pixelData[animID];

        for(int x=0; x < Tile::WIDTH; x++) {
            for(int y=0; y < Tile::HEIGHT; y++) {
                int pixelIndex = y * Tile::WIDTH + x;
                uint16_t sourceIdx = _data[pixelIndex];

                PalletteAnimation pixelAnimation = animationLengthForIndex(sourceIdx);
                sourceIdx = (pixelAnimation.start + (sourceIdx+animID-pixelAnimation.start)%pixelAnimation.length);
                sourceIdx *= 4;

                // GBR -> RGBA
                int targetIdx = 4*(y * Tile::WIDTH + x);
                animationTile[targetIdx+0] = palette[sourceIdx+2];
                animationTile[targetIdx+1] = palette[sourceIdx+1];
                animationTile[targetIdx+2] = palette[sourceIdx+0];
                animationTile[targetIdx+3] = sourceIdx == 0 ? 0 : 0xFF;
            }
        }


        GLuint &currentTexture = *(_textures+animation.length - animID -1);
        currentTexture = r->createTexture(animationTile, Tile::SIZE);

        delete [] pixelData[animID];
    }
    
    delete [] pixelData;
}

GLuint Tile::getTexture(unsigned ticks)
{
    return *(_textures + ((ticks/2) % _textureCount));
}

void Tile::teardownTextures()
{

}
#pragma mark -
PalletteAnimation Tile::animationLengthForIndex(int index)
{
#define IndexInRange(_A_, _B_) if(index >= _A_ && index <= _B_) return {_A_, _B_ - _A_ +1 };

    if(context->getGameStyle() == GameStyleYoda)
    {
        IndexInRange(0x0a, 0x0f); // confirmed on 0x5f upper right hand corner
        IndexInRange(0xc0, 0xc3);
        IndexInRange(0xc4, 0xc5);
        IndexInRange(0xc6, 0xc7);
        IndexInRange(0xc8, 0xc9);
        IndexInRange(0xca, 0xcb);
        IndexInRange(0xcc, 0xcd);
        IndexInRange(0xce, 0xcf);
        IndexInRange(0xd7, 0xdf);
        IndexInRange(0xe0, 0xe4); // checkd on 0x217 (yoda's hut interior)
        IndexInRange(0xe5, 0xed); // checkd on 0x217 (yoda's hut interior)
        IndexInRange(0xee, 0xf5);
    } else {
        IndexInRange(0xe0, 0xe4);
        IndexInRange(0xe5, 0xed);
        IndexInRange(0xee, 0xf3);
        IndexInRange(0xf4, 0xf5);
        // IndexInRange(0xf6, 0xff); // all black, not really worth animating
    }

    // rest black? one white…
    return {index,1};
}

PalletteAnimation Tile::effectivePaletteAnimation()
{
    PalletteAnimation animation = {0,0};
    for(int i=0; i < Tile::SIZE; i++) {
        PalletteAnimation pixelAnimation = animationLengthForIndex(_data[i]);
        if(pixelAnimation.length > animation.length)
            animation = pixelAnimation;
    }
    return animation;
}
