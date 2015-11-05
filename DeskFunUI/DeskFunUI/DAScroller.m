//
//  DAOldScroller.m
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAScroller.h"
#import "DAWindowTheme.h"
@interface DAScroller ()
@property (assign) NSClipView *reflectedClipView;

static void DrawBackgroundPattern(void *info, CGContextRef context);
static void ReleaseBackgroundPattern(void *info);
@property DABezelView *barView;
@end
@implementation DAScroller

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if(self){
        [self _commonInit];
    }
    return self;
}

- (void)_commonInit
{
    self.barView = [[DABezelView alloc] init];
    self.barView.bezelWidth = 1.0;
}

- (BOOL)isFlipped
{
    return NO;
}
#pragma mark -
- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    NSRect bounds = NSInsetRect(self.bounds, self.bezelWidth, self.bezelWidth);

    static const CGPatternCallbacks callbacks = {0, &DrawBackgroundPattern, &ReleaseBackgroundPattern};
    CGPatternRef    pattern       = CGPatternCreate(NULL, CGRectMake(0.0, 0.0, 2, 2), CGAffineTransformMake(1.0, 0.0, 0.0, 1.0, 0.0, 0.0), 2, 2, kCGPatternTilingConstantSpacing, YES, &callbacks);
    CGColorSpaceRef space         = CGColorSpaceCreatePattern(NULL);
    CGFloat         components[1] = {1.0};

    CGColorRef patternColor = CGColorCreateWithPattern(space, pattern, components);
    NSColor *color = [NSColor colorWithCGColor:patternColor];
    [color setFill];
    NSRectFill(bounds);

    CGColorSpaceRelease(space);
    CGPatternRelease(pattern);
    CGColorRelease(patternColor);

    /*
    NSRect documentRect = self.reflectedClipView.documentRect;
    NSRect visibleRect  = self.reflectedClipView.bounds;

    if(NSHeight(documentRect)==0) return;

    NSRect barRect = bounds;
    barRect.size.height = NSMinX(bounds) + MAX(NSHeight(visibleRect)/NSHeight(documentRect) * NSHeight(bounds), 16);
    barRect.origin.y    = NSMinY(bounds) + NSHeight(bounds)-barRect.size.height - NSMinY(visibleRect)/NSHeight(documentRect) * NSHeight(bounds);

    barRect = [self convertRectFromBacking:[self convertRectToBacking:barRect]];
     */
}

static void DrawBackgroundPattern(void *info, CGContextRef context)
{
    float cellWidth = 2;
    float cellHeight = 2;

    //Chequered Flag
    //Fill the entire background with white color
    CGContextSetFillColorWithColor(context, [NSColor whiteColor].CGColor);
    CGContextFillRect(context, CGRectMake(0.0, 0.0, cellWidth, cellHeight));

    CGContextSetFillColorWithColor(context, [NSColor colorWithDeviceRed:212/255.0 green:208/255.0 blue:200/255.0 alpha:1.0].CGColor);
    //Fill the top left quarter of the cell with black
    CGContextFillRect(context, CGRectMake(0.0, 0.0, cellWidth/2.0, cellHeight/2.0));

    //Fill the bottom right quarter of the cell with black
    CGContextFillRect(context, CGRectMake(cellWidth/2.0, cellHeight/2.0, cellWidth/2.0, cellHeight/2.0));
}

static void ReleaseBackgroundPattern(void *info)
{}
#pragma mark -
- (void)reflectScrolledClipView:(NSClipView *)aClipView
{
    self.reflectedClipView = aClipView;
}
@end
