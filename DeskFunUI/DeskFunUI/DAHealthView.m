//
//  DAHealthView.m
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//
#import <QuartzCore/QuartzCore.h>

#import "DAHealthView.h"

#import "DAWindowTheme.h"
@interface DAHealthView ()
- (void)_commonInit;
@end

@implementation DAHealthView
- (id)init
{
    self = [super init];
    if(self)
    {
        [self _commonInit];
    }
    return self;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self)
    {
        [self _commonInit];
    }
    return self;
}

- (void)_commonInit
{
    self.bezelWidth    = 2;

    self.health = 1.78;

    self.goodColor     = [NSColor greenColor];
    self.mediumColor   = [NSColor yellowColor];
    self.badColor      = [NSColor redColor];
    self.criticalColor = [NSColor blackColor];

    CABasicAnimation *animation = [CABasicAnimation animation];
    [self setAnimations:@{@"health": animation}];
}

- (void)setHealth:(CGFloat)health
{
    _health = health;
    [self setNeedsDisplay:YES];
}


- (BOOL)isOpaque
{
    return NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
    DAWindowTheme *theme    = [DAWindowTheme sharedTheme];
    NSBezierPath *bezelClip = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
    [bezelClip setClip];

    NSBezierPath *split = [NSBezierPath bezierPath];
    [split moveToPoint:(NSPoint){0,0}];
    [split lineToPoint:(NSPoint){NSMaxX(self.bounds),NSMaxY(self.bounds)}];
    [split lineToPoint:(NSPoint){NSMaxX(self.bounds),0}];
    [split lineToPoint:(NSPoint){0,0}];

    [[theme bezelColorLight] setFill];
    [split fill];

    split = [NSBezierPath bezierPath];
    [split moveToPoint:(NSPoint){0,0}];
    [split lineToPoint:(NSPoint){0,NSMaxY(self.bounds)}];
    [split lineToPoint:(NSPoint){NSMaxX(self.bounds),NSMaxY(self.bounds)}];
    [split lineToPoint:(NSPoint){0,0}];

    [[theme bezelColorDark] setFill];
    [split fill];

//    return;
    NSColor *indicatorColor  = nil;
    NSColor *portionColor    = nil;

    CGFloat relativeHealth = self.health;

    if(self.health > 2)
    {
        portionColor = self.goodColor;
        indicatorColor = self.mediumColor;

        relativeHealth -= 2;
    }
    else if(self.health > 1)
    {
        portionColor = self.mediumColor;
        indicatorColor = self.badColor;

        relativeHealth -= 1;
    }
    else
    {
        portionColor = self.badColor;
        indicatorColor = self.criticalColor;
    }

    bezelClip = [NSBezierPath bezierPathWithOvalInRect:NSInsetRect(self.bounds, self.bezelWidth, self.bezelWidth)];
    [bezelClip setClip];
    [indicatorColor setFill];
    [bezelClip fill];

    NSBezierPath *portionClip = [NSBezierPath bezierPath];
    [portionClip moveToPoint:(NSPoint){NSMidX(self.bounds), NSMidY(self.bounds)}];
    [portionClip appendBezierPathWithArcWithCenter:(NSPoint){NSMidX(self.bounds), NSMidY(self.bounds)} radius:NSMidX(self.bounds)-self.bezelWidth+0.5 startAngle:90 endAngle:90+relativeHealth*360.0];
    [portionClip setClip];
    [portionColor setFill];
    [portionClip fill];
}

#pragma mark - Debug Interaction
- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{
    return YES;
}

- (void)mouseUp:(NSEvent *)theEvent
{
    CGFloat newValue = (rand() / (double)RAND_MAX)*3;
    [[self animator] setHealth:newValue];
}
@end
