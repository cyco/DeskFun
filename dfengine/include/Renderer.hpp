//
//  Renderer.hpp
//  DeskFun Engine
//

//
//

#ifndef Renderer_cpp
#define Renderer_cpp

#include <stdio.h>
typedef unsigned int TileID;

class Renderer {
public:
    virtual void renderTile(TileID texture, float x, float y, float l) = 0;
    virtual TileID createTexture(void *data, size_t length) = 0;
    virtual void setup() = 0;
};
#endif /* Renderer_cpp */
