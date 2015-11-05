//
//  PaletteView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "PaletteView.h"
#import "AppDelegate.h"
#import "GameDataParser.hpp"
#import "palette.h"
@interface PaletteView () {
    NSPoint p;
}
@end

@implementation PaletteView
- (void)drawRect:(NSRect)dirtyRect
{
    AppDelegate *dele = (AppDelegate *)[NSApp delegate];
    GameDataParser *data = [dele GameDataParser];
    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }

    int width = 0x10, height = 0x10;
    CGFloat factor = [self scale];
    for(int i=0; i < 0x100; i++)
    {
        int colorIndex = i;
        unsigned char b = palette[colorIndex*4+0];
        unsigned char g = palette[colorIndex*4+1];
        unsigned char r = palette[colorIndex*4+2];
        __unused unsigned char rest = palette[colorIndex*4+3];

        CGFloat alpha = i == 0 ? 0.0 : 1.0;

        [[NSColor colorWithDeviceRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:alpha] setFill];
        NSRectFill(NSMakeRect((i%height)*factor+1, floorf(i/width)*factor+1, factor, factor));
    }

    [[[NSColor redColor] colorWithAlphaComponent:0.3] setFill];
    CGFloat scale = [self scale];
    NSRect r = NSMakeRect(p.x*scale +1, p.y*scale +1, 1*scale, 1*scale);
    NSRectFillUsingOperation(r, NSCompositeSourceOver);
}

#pragma mark -
- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{
    return YES;
}

- (void)updateTrackingAreas
{
    [[self trackingAreas] enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        [self removeTrackingArea:obj];
    }];

    [self addTrackingArea:[[NSTrackingArea alloc] initWithRect:[self bounds] options:NSTrackingMouseMoved|NSTrackingActiveInKeyWindow owner:self userInfo:nil]];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSPoint locationInWindow = [theEvent locationInWindow];
    NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

    int x = locationInView.x / [self scale];
    int y = locationInView.y / [self scale];

    p.x = x;
    p.y = y;

    AppDelegate *dele = (AppDelegate *)[NSApp delegate];
    GameDataParser *data = [dele GameDataParser];
    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }


    int index = y * 0x10 + x;
    unsigned char b = palette[index*4+0];
    unsigned char g = palette[index*4+1];
    unsigned char r = palette[index*4+2];
    unsigned char rest = palette[index*4+3];

    NSLog(@"0x%02x | 0x%02x, 0x%02x, 0x%02x, 0x%02x", index, b, g, r, rest);

    [self setNeedsDisplay:YES];
}

- (CGFloat)scale
{
    return 8.0;
}

@end
