//
//  DAMapView.m
//  DA
//
//  Created by Christoph Leimbrock on 10/15/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DASceneView.h"
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>

#include <random>
#import <Carbon/Carbon.h>

#import "CocoaInputManager.hpp"
#import "OpenGLRenderer.hpp"

@interface DASceneView ()
{
    CVDisplayLinkRef displayLink;
    CocoaInputManager _InputManager;
    NSTimer *timer;
    NSTimeInterval deltaTime;
}
@end

@implementation DASceneView

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (void)_commonInit
{
    srandom((unsigned int)time(NULL));
}

#pragma mark -
- (void)viewDidMoveToSuperview
{
    if ([self superview]) {
        if (timer) {
            [timer invalidate];
            timer = Nil;
        }

        NSTimeInterval interval = 1/10.0;
        printf("Timer duration %f seconds\n", interval);
        timer = [NSTimer timerWithTimeInterval:interval target:self selector:@selector(tick:) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];

    } else {
        [timer invalidate];
        timer = Nil;
    }
}

- (void)tick:(id)sender
{
    @synchronized(self)
    {
        NSPoint locationOnScreen = [NSEvent mouseLocation];
        NSPoint locationOnWindow = [[self window] convertRectFromScreen:(NSRect){.origin=locationOnScreen}].origin;
        NSPoint locationInView = [self convertPoint:locationOnWindow fromView:nil];

        [self _updateMouseLocationWithViewPoint:locationInView];
        if (self && self->_engine)
            self->_engine->update(1);
    }
}

- (void)setEngine:(Engine *)engine
{
    _engine = engine;
    _engine->InputManager = &_InputManager;
}
#pragma mark - Interaction -
- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{    return YES;
}

- (BOOL)resignFirstResponder
{
    return YES;
}

- (void)updateTrackingAreas
{
    NSTrackingArea *area;
    while ((area = [[self trackingAreas] lastObject]))
        [self removeTrackingArea:area];

    const NSRect bounds = [self bounds];
    const NSRect trackingRect = NSInsetRect(bounds, -10, -10);
    const NSTrackingAreaOptions options = NSTrackingMouseMoved|NSTrackingActiveInKeyWindow|NSTrackingCursorUpdate;
    area = [[NSTrackingArea alloc] initWithRect:trackingRect options:options owner:self userInfo:nil];
    [self addTrackingArea:area];
}

#pragma mark - Left Mouse
- (void)mouseDown:(NSEvent *)theEvent
{
    _InputManager._leftMouseDown = true;
    [self _updateMouseLocation:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    [self _updateMouseLocation:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    _InputManager._leftMouseDown = true;
    [self _updateMouseLocation:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    _InputManager._leftMouseDown = false;
    [self _updateMouseLocation:theEvent];
}

#pragma mark - Right Mouse
- (void)rightMouseDown:(NSEvent *)theEvent
{
    _InputManager._rightMouseDown = true;
    [self _updateMouseLocation:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    _InputManager._rightMouseDown = true;
    [self _updateMouseLocation:theEvent];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    _InputManager._rightMouseDown = false;
    [self _updateMouseLocation:theEvent];
}

#pragma mark - Key Handling
- (void)keyDown:(NSEvent *)theEvent
{
    [self _addInputFromKey:[theEvent keyCode] released:NO];
}

- (void)keyUp:(NSEvent *)theEvent
{
    [self _addInputFromKey:[theEvent keyCode] released:YES];
}

- (void)flagsChanged:(NSEvent *)theEvent
{
    static NSUInteger lastModifierState = 0;
    NSUInteger flagState = [theEvent modifierFlags];
    NSUInteger change = flagState ^ lastModifierState;

    if((change&NSShiftKeyMask) != 0) {
        [self _addInputFromKey:kVK_Shift released:(flagState&NSShiftKeyMask) == 0];
    }

    lastModifierState = flagState;
}

#pragma mark - Interaction Helpers
- (void)_updateMouseLocation:(NSEvent*)theEvent
{
    const NSPoint locationInWindow = [theEvent locationInWindow];
    const NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

    [self _updateMouseLocationWithViewPoint:locationInView];
}

- (void)_updateMouseLocationWithViewPoint:(NSPoint)locationInView
{
    const NSRect bounds = [self bounds];
    _InputManager._mouseLocation = (Pointf){
        .x=(float)(locationInView.x / NSWidth(bounds)),
        .y=(float)(1.0 - locationInView.y / NSHeight(bounds))
    };
}

- (void)_addInputFromKey:(unsigned short)keycode released:(BOOL)released
{
    switch (keycode) {
        case kVK_Shift:  _InputManager.changeKey(InputManager::Drag,   !released); break;
        case kVK_Space:  _InputManager.changeKey(InputManager::Attack, !released); break;

        case kVK_ANSI_P: _InputManager.changeKey(InputManager::Pause, !released); break;
        case kVK_ANSI_L: _InputManager.changeKey(InputManager::Map,    !released); break;

        case kVK_LeftArrow:  _InputManager.changeKey(InputManager::Left, !released); break;
        case kVK_RightArrow: _InputManager.changeKey(InputManager::Right, !released); break;
        case kVK_UpArrow:    _InputManager.changeKey(InputManager::Up, !released); break;
        case kVK_DownArrow:  _InputManager.changeKey(InputManager::Down, !released); break;
    }
}

#pragma mark - Drawing
- (void)prepareOpenGL
{
    [self setWantsBestResolutionOpenGLSurface:NO];
    [super prepareOpenGL];

    [self _setupOpenGL];
    [self _setupDisplayLink];
}

- (void)_setupOpenGL
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
}

- (void)_setupDisplayLink
{
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    CVDisplayLinkStart(displayLink);
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
    DASceneView* view = (__bridge DASceneView *)displayLinkContext;
    @autoreleasepool {
        view->deltaTime = 1.0 / (outputTime->rateScalar * (double)outputTime->videoTimeScale / (double)outputTime->videoRefreshPeriod);

        [view setNeedsDisplay:YES];
    }
    return kCVReturnSuccess;
}

- (BOOL)isOpaque
{
    return YES;
}

- (void)reshape
{
    NSRect bounds = [self bounds];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, 9.0, 9.0, 0.0, 0.0, 10.0);
    glViewport(0, 0, NSWidth(bounds), NSHeight(bounds));
}

- (void)drawRect:(NSRect)dirtyRect
{
#define CheckError()    { GLenum err = glGetError(); if (err != GL_NO_ERROR) printf("OpenGL error %08x\n", err); }

    glEnable(GL_TEXTURE_2D); CheckError();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);CheckError();

    glEnable(GL_BLEND);CheckError();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);CheckError();
    glBlendEquation(GL_FUNC_ADD); CheckError();

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); CheckError();

    if(_engine != NULL) _engine->render(deltaTime);

    glBindTexture(GL_TEXTURE_2D, 0);CheckError();
    glDisable(GL_BLEND);   CheckError();
    glDisable(GL_TEXTURE_2D);CheckError();

    glFlush();CheckError();
}

@end
