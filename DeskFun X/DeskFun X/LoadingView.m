//
//  LoadingView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "LoadingView.h"

#import <DeskFunUI/DAProgressIndicator.h>

#define ProgressIndicatorClass DAProgressIndicator
@interface LoadingView ()
@property (strong) NSImage *loadingImage;
@property (assign) ProgressIndicatorClass *progressIndicator;
@end

@implementation LoadingView

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if(self)
    {
        [self _setupProgressIndicator];
    }
    return self;
}

- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if(self)
    {
        [self _setupProgressIndicator];
    }
    return self;
}

- (void)_setupProgressIndicator
{
    const NSRect frame = NSMakeRect(0, 265, 269, 13);
    ProgressIndicatorClass *indicator = [[ProgressIndicatorClass alloc] initWithFrame:frame];
    [indicator setIndeterminate:NO];
    [indicator setMinValue:0.0];
    [indicator setDoubleValue:0.0];
    [indicator setMaxValue:1.0];

    [indicator setAutoresizingMask:NSViewMinXMargin|NSViewMaxXMargin|NSViewMaxYMargin];
    _progressIndicator = indicator;
    [self addSubview:indicator];
}
#pragma mark -
- (void)setLoadingImageWithData:(const unsigned char*)data size:(size_t)size andPalette:(const unsigned char*)pal
{
    int width, height;
    width = height = sqrt(size);

    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
    [image lockFocus];
    for(int i=0; i < size; i++)
    {
        uint16_t colorIndex = data[i];
        unsigned int palIndex = colorIndex * 4;

        unsigned char b = pal[palIndex+0];
        unsigned char g = pal[palIndex+1];
        unsigned char r = pal[palIndex+2];
        CGFloat alpha = colorIndex == 0 ? 0.0 : 1.0;

        [[NSColor colorWithDeviceRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:alpha] setFill];

        CGFloat x = i % width;
        CGFloat y = height - i / height - 1.0;
        NSRectFill(NSMakeRect(x, y, 1, 1));
    }

    [image unlockFocus];
    [self setLoadingImage:image];
    [self setNeedsDisplay:YES];
}

- (void)setProgress:(CGFloat)progress
{
    [_progressIndicator setDoubleValue:progress];
}

- (double)progress
{
    return [_progressIndicator doubleValue];
}
#pragma mark - Interaction
- (void)mouseDown:(NSEvent *)theEvent
{
    double progress = [self progress];

    if(progress == 1.0)
        progress = 0.0;
    else progress += 0.1;

    [self setProgress:progress];
}

#pragma mark -
- (void)resizeSubviewsWithOldSize:(NSSize)oldSize
{
    const NSRect bounds = [self bounds];

    const CGFloat hSpace = 9.0;
    const CGFloat indicatorHeight = 13.0;
    const CGFloat vSpace = 10.0;

    const NSRect progressFrame = NSMakeRect(hSpace, vSpace, NSWidth(bounds)-2*hSpace, indicatorHeight);
    [_progressIndicator setFrame:progressFrame];
}

- (void)drawRect:(NSRect)dirtyRect
{
    const NSRect bounds = [self bounds];

    [[NSColor blackColor] setFill];
    NSRectFill(bounds);

    if(_loadingImage)
        [_loadingImage drawInRect:bounds fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}
@end
