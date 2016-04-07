//
//  Tile.h
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DAParser__Tile__
#define __DAParser__Tile__

#include <stdio.h>

#ifdef __cplusplus
#include <stdlib.h>
#include <string>
#endif

#include "OpenGL.h"
#include "GameObject.hpp"
#include "Renderer.hpp"

using namespace std;

typedef struct { int start; int length; } PalletteAnimation;

class Tile : GameObject
{
public: // Serialization
    Tile(FILE *file);
    ~Tile(void);

    size_t write(void* buffer);

public:
    uint32_t _specs;
    uint8_t* _data;


public:
    static const char NAMES_CATEGORY[];
    const static int HEIGHT = 32;
    const static int WIDTH  = 32;
    const static int SIZE   = Tile::WIDTH * Tile::HEIGHT;

    static void ReadNamesFromFile(FILE *file, range range, vector<Tile*> &tiles);

    Tile(void);

    string   name = string();
    uint16_t index;

    void readName(FILE *f);
    size_t writeName(void* buffer);

    bool isWalkable() const;
    bool isDraggable() const;

    bool isLocatorTile() const;
    uint16_t getLocatorType() const;

    void setupTextures(Renderer *r);
    GLuint getTexture(unsigned ticks);
    void teardownTextures();
public:

    GLuint *_textures = NULL;
    unsigned _textureCount = 1;

    PalletteAnimation effectivePaletteAnimation();
    PalletteAnimation animationLengthForIndex(int index);

public: // debugging
    Tile(uint16_t id, uint32_t specs, uint8_t *data);

    uint32_t debugGetSpecs(){
        return _specs;
    }
    uint8_t *debugGetData(){
        return _data;
    }
    bool debugIsAnimated = false;
};

#endif /* defined(__DAParser__Tile__) */
