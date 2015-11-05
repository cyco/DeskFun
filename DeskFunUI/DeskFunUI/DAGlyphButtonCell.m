//
//  DAButtonCell.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DAGlyphButtonCell.h"

@implementation DAGlyphButtonCell

#pragma mark - Sizing
- (NSSize)cellSize
{
    return NSMakeSize(16, 16);
}

- (NSSize)cellSizeForBounds:(NSRect)aRect
{
    return NSMakeSize(16, 16);
}
#pragma mark - Drawing
- (void)drawWithFrame:(NSRect)frame inView:(NSView *)controlView
{
    // background
    [[NSColor colorWithDeviceWhite:0.75 alpha:1.0] setFill];
    NSRectFill(frame);

    const BOOL highlighted = [self isHighlighted];
    NSColor *topHighlight    = highlighted ? [NSColor colorWithDeviceWhite:0.5 alpha:1.0] : [NSColor whiteColor];
    NSColor *bottomHighlight = highlighted ? [NSColor whiteColor] : [NSColor colorWithDeviceWhite:0.5 alpha:1.0];

    // highlight left, top
    [topHighlight setFill];
    NSRectFill(NSMakeRect(NSMinX(frame)+1, NSMinY(frame)+1, NSWidth(frame)-2, 1));
    NSRectFill(NSMakeRect(NSMinX(frame)+1, NSMinY(frame)+2, 1, NSHeight(frame)-3));

    // shadows bottom, right
    [bottomHighlight setFill];
    NSRectFill(NSMakeRect(NSMinX(frame)+1, NSMaxY(frame)-2, NSWidth(frame)-2, 1));
    NSRectFill(NSMakeRect(NSMaxX(frame)-2, NSMinY(frame)+2, 1, NSHeight(frame)-4));

    // border
    [[NSColor blackColor] setStroke];
    [[NSBezierPath bezierPathWithRect:NSInsetRect(frame, 0.5, 0.5)] stroke];

    NSRect imageRect = NSInsetRect(frame, 4.0, 4.0);
    if([self isEnabled])
    {
        imageRect.origin.x += 1;
        imageRect.origin.y += 1;
    }

    NSBezierPath *imagePath = [self imagePathWithGlyphRect:imageRect];
    if(!highlighted)
    {
        [[NSColor colorWithDeviceWhite:0.5 alpha:1.0] setFill];
        [imagePath fill];
    }

    if([self isEnabled])
    {
        NSAffineTransform *transform = [NSAffineTransform transform];
        [transform translateXBy:-1 yBy:-1];
        [imagePath transformUsingAffineTransform:transform];

        // draw image in black
        [[NSColor blackColor] setFill];
        [imagePath fill];
    }
}

- (NSBezierPath*)imagePathWithGlyphRect:(NSRect)rect
{
    return nil;
}

@end
