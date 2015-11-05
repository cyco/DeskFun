//
//  PauseScene.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "PauseScene.hpp"
#include "OpenGL.h"

#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#include "InputManager.hpp"
#include "Engine.hpp"

void PauseScene::init(void)
{
    texture = this->LoadTexture(CFSTR("pause"));
}

void PauseScene::cleanup(void)
{}

bool PauseScene::render(double delta)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    for (int x=0; x < 9; x++) {
        for (int y=0; y < 9; y++) {
#ifdef GL_QUADS
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex2f(x+0.0, y+0.0);
            glTexCoord2f(1.0, 0.0); glVertex2f(x+1.0, y+0.0);
            glTexCoord2f(1.0, 1.0); glVertex2f(x+1.0, y+1.0);
            glTexCoord2f(0.0, 1.0); glVertex2f(x+0.0, y+1.0);
            glEnd();
#endif
        }
    }

    return true;
}

bool PauseScene::update(double delta)
{
    InputManager* InputManager = _engine->InputManager;

    bool pauseKeyDown    = (InputManager->getKeyStates()       & InputManager::Pause) != 0;
    bool pauseKeyChanged = (InputManager->getKeyStateChanges() & InputManager::Pause) != 0;
    if(pauseKeyDown && pauseKeyChanged) {
        _engine->popScene();
    }

    return false;
}
#pragma mark -
GLuint PauseScene::LoadTexture(CFStringRef name)
{
    /*
    CFURLRef texture_url = CFBundleCopyResourceURL(CFBundleGetMainBundle(), name, CFSTR("png"), NULL);

    CGImageSourceRef imageSource = CGImageSourceCreateWithURL(texture_url, NULL);
    CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
    CFRelease(imageSource);
    size_t width  = CGImageGetWidth (image);
    size_t height = CGImageGetHeight(image);
    CGRect rect = CGRectMake(0.0f, 0.0f, width, height);

    void *imageData = malloc(width * height * 4);
    CGColorSpaceRef colourSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate(imageData, width, height, 8, width * 4, colourSpace, kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst);
    CFRelease(colourSpace);
    CGContextTranslateCTM(ctx, 0, height);
    CGContextScaleCTM(ctx, 1.0f, -1.0f);
    CGContextSetBlendMode(ctx, kCGBlendModeCopy);
    CGContextDrawImage(ctx, rect, image);
    CGContextRelease(ctx);
    CFRelease(image);

    GLuint glName;
    glGenTextures(1, &glName);
    glBindTexture(GL_TEXTURE_2D, glName);

#ifdef GL_UNPACK_ROW_LENGTH
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)width);
#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef GL_RGBA8
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (int)width, (int)height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, imageData);
#endif
    free(imageData);
    CFRelease(texture_url);

    return glName;
     */
    return 0;
}