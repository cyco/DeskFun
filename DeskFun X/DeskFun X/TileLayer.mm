//
//  TileLayer.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "TileLayer.h"
#import "AppDelegate.h"
#import <OpenGL/CGLContext.h>
#import <OpenGL/gl.h>

#import "Palette.h"
#import "OpenGLRenderer.hpp"
#import "DAGameWindowController.h"
@interface TileLayer ()
{
    CGLContextObj obj;
}
@property unsigned short ticks;
@end

@implementation TileLayer

+ (instancetype)layer
{
    TileLayer *layer = [super layer];

    layer.ticks = 0;
    layer.tile = NULL;

    return layer;
}

- (void)tick
{
    _ticks++;
    [self setNeedsDisplay];
}

- (void)setTile:(Tile *)tile
{
    _tile = tile;
}
#pragma mark -
- (CGLPixelFormatObj)copyCGLPixelFormatForDisplayMask:(uint32_t)mask
{
    return [super copyCGLPixelFormatForDisplayMask:mask];
}

- (CGLContextObj)copyCGLContextForPixelFormat:(CGLPixelFormatObj)pixelFormat
{
    CGLError error = CGLCreateContext(pixelFormat, [(DAGameWindowController*)[(AppDelegate*)[NSApp delegate] gameWindowController] renderer]->ctx, &obj);
    if(error != kCGLNoError) NSLog(@"Error: %s", CGLErrorString(error));
    return obj;
}

- (BOOL)canDrawInCGLContext:(CGLContextObj)glContext pixelFormat:(CGLPixelFormatObj)pixelFormat forLayerTime:(CFTimeInterval)timeInterval displayTime:(const CVTimeStamp *)timeStamp
{
    return YES;
}
#pragma mark -
- (void)drawInCGLContext:(CGLContextObj)cgl_ctx pixelFormat:(CGLPixelFormatObj)pixelFormat forLayerTime:(CFTimeInterval)timeInterval displayTime:(const CVTimeStamp *)timeStamp
{
    CGLSetCurrentContext(cgl_ctx);
    if(_tile)
        ;// _tile->setupTextures();

    const CGRect frame = [self frame];
#define CheckError()    { GLenum err = glGetError(); if (err != GL_NO_ERROR) printf("OpenGL error %08x\n", err); }

    glClearColor(0.0, 0.0, 0.0, 1.0); CheckError();

    glMatrixMode(GL_PROJECTION);CheckError();
    glLoadIdentity();CheckError();


    glViewport(0, 0, NSWidth(frame), NSHeight(frame));CheckError();
    glOrtho(0.0, 1.0, 1.0, 0.0, 10.0, 0.0);CheckError();

    glEnable(GL_TEXTURE_2D);CheckError();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);CheckError();

    glEnable(GL_BLEND);CheckError();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);CheckError();
    glBlendEquation(GL_FUNC_ADD);CheckError();

    glClear(GL_COLOR_BUFFER_BIT);CheckError();

    glDisable(GL_BLEND);

    if(_tile)
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        OpenGLRenderer *renderer = [(DAGameWindowController*)[(AppDelegate*)[NSApp delegate] gameWindowController] renderer];CheckError();
        renderer->renderTile(_tile->getTexture(0), 0, 0, 0);CheckError();

    }

    glBindTexture(GL_TEXTURE_2D, 0);CheckError();
    glDisable(GL_TEXTURE_2D);CheckError();
    glDisable(GL_BLEND);CheckError();

    [super drawInCGLContext:cgl_ctx pixelFormat:pixelFormat forLayerTime:timeInterval displayTime:timeStamp];
}

- (void)releaseCGLContext:(CGLContextObj)glContext
{
}

- (void)releaseCGLPixelFormat:(CGLPixelFormatObj)pixelFormat
{
    [super releaseCGLPixelFormat:pixelFormat];
}

@end
