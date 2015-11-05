//
//  OpenGLRenderer.cpp
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#include "OpenGLRenderer.hpp"
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import "Tile.hpp"

void OpenGLRenderer::renderTile(TileID texture, float x, float y, float l) {
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(x+0.0, y+0.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(x+1.0, y+0.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(x+1.0, y+1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(x+0.0, y+1.0);
    glEnd();
}

TileID OpenGLRenderer::createTexture(void *data, size_t length) {
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)Tile::WIDTH);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, Tile::HEIGHT, Tile::WIDTH, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, Tile::HEIGHT, Tile::WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return texName;
}

void OpenGLRenderer::setup() {
    this->ctx = CGLGetCurrentContext();
}