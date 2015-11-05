//
//  DABezelView.m
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DABezelView.h"
#import "DAWindowTheme.h"

@implementation DABezelView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.bezelWidth = 2.0;
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self)
    {
        self.bezelWidth = 2.0;
    }
    return self;
}


- (id)init
{
    self = [super init];
    if(self)
    {
        self.bezelWidth = 2.0;
    }
    return self;
}

- (BOOL)isOpaque
{
    return [[[DAWindowTheme sharedTheme] backgroundColor] alphaComponent] == 1.0;;
}

- (void)drawRect:(NSRect)dirtyRect
{
    DAWindowTheme *theme = [DAWindowTheme sharedTheme];
    CGFloat bezelWidth = self.bezelWidth;

    NSRect bounds = self.bounds;
    CGFloat maxY = NSMaxY(bounds);
    CGFloat maxX = NSMaxX(bounds);

    NSBezierPath *path = [NSBezierPath bezierPath];
    [path moveToPoint:NSMakePoint(0, 0)];
    [path lineToPoint:NSMakePoint(0, maxY)];
    [path lineToPoint:NSMakePoint(maxX, maxY)];

    [path lineToPoint:NSMakePoint(maxX-bezelWidth, maxY-bezelWidth)];
    [path lineToPoint:NSMakePoint(bezelWidth, maxY-bezelWidth)];
    [path lineToPoint:NSMakePoint(bezelWidth, bezelWidth)];
    [path lineToPoint:NSMakePoint(0, 0)];

    [self.swapColors ? [theme bezelColorLight] : [theme bezelColorDark] set];
    [path fill];

    path = [NSBezierPath bezierPath];
    [path moveToPoint:NSMakePoint(0, 0)];
    [path lineToPoint:NSMakePoint(maxX, 0)];
    [path lineToPoint:NSMakePoint(maxX, maxY)];

    [path lineToPoint:NSMakePoint(maxX-bezelWidth, maxY-bezelWidth)];
    [path lineToPoint:NSMakePoint(maxX-bezelWidth, bezelWidth)];
    [path lineToPoint:NSMakePoint(bezelWidth, bezelWidth)];
    [path lineToPoint:NSMakePoint(0, 0)];

    [self.swapColors ? [theme bezelColorDark] : [theme bezelColorLight] set];
    [path fill];

    [self.backgroundColor ?: [theme backgroundColor] setFill];
    NSRectFill(NSInsetRect(self.bounds, bezelWidth, bezelWidth));
}

@end
