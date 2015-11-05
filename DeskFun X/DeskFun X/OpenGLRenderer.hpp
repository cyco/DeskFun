//
//  OpenGLRenderer.hpp
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#ifndef OpenGLRenderer_cpp
#define OpenGLRenderer_cpp

#include <stdio.h>
#include "Renderer.hpp"
#import <OpenGL/CGLCurrent.h>

class OpenGLRenderer : public Renderer
{
public:
    virtual void renderTile(TileID texture, float x, float y, float l) override;
    virtual TileID createTexture(void *data, size_t length) override;
    virtual void setup() override;

public:
    CGLContextObj ctx;
};
#endif /* OpenGLRenderer_cpp */
