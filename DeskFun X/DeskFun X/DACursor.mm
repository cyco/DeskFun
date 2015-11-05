//
//  DACursor.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DACursor.h"
#include "Direction.hpp"

@interface DACursor ()
+ (instancetype)cursorWithKey:(NSString*)key;
@end

@implementation DACursor

+ (instancetype)cursorWithKey:(NSString*)key
{
    static NSMutableDictionary *cursors = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        cursors = [NSMutableDictionary dictionaryWithCapacity:9];
    });

    if([cursors objectForKey:key])
        return [cursors objectForKey:key];

    NSDictionary *definitions = [self cursorDefinition];
    NSDictionary *cursorDefinition = [definitions objectForKey:key];

    NSString *imageName = [cursorDefinition objectForKey:@"resource"];
    NSURL *imageURL = [[NSBundle mainBundle] URLForResource:imageName withExtension:@"pdf"];
    NSImage *image = [[NSImage alloc] initWithContentsOfURL:imageURL];

    NSString *hotspotString = [cursorDefinition objectForKey:@"hotspot"];
    NSPoint hotspot = NSPointFromString(hotspotString);

    NSImage *resultImage = [[NSImage alloc] initWithSize:[image size]];
    for (int scale = 1; scale <= 8; scale++) {
        NSAffineTransform *xform = [[NSAffineTransform alloc] init];
        [xform scaleBy:scale];
        id hints = @{ NSImageHintCTM: xform };
        CGImageRef rasterCGImage = [image CGImageForProposedRect:NULL context:nil hints:hints];
        NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:rasterCGImage];
        [rep setSize:[image size]];
        [resultImage addRepresentation:rep];
    }

    DACursor *cursor = [[DACursor alloc] initWithImage:resultImage hotSpot:hotspot];
    [cursors setObject:cursor forKey:key];
    return cursor;
}

+ (NSDictionary*)cursorDefinition
{
    static NSDictionary *definition = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSURL *url = [[NSBundle mainBundle] URLForResource:@"cursors" withExtension:@"plist"];
        definition = [NSDictionary dictionaryWithContentsOfURL:url];
    });
    return definition;
}

#pragma mark -
+ (NSCursor*)defaultCursor
{
    return [NSCursor arrowCursor];
}

+ (instancetype)blockingCursor
{
    return [self cursorWithKey:@"block"];
}

#pragma mark -
+ (instancetype)cursorWithDirection:(float)direction
{
    if(isnan(direction))
        return [self blockingCursor];

    if(isinf(direction))
        return nil;

    direction = Direction::Confine(direction);
    NSString *key = [self keyForDirection:direction];
    return [self cursorWithKey:key];
}

+ (NSString*)keyForDirection:(int)direction
{
    switch (direction) {
        case Direction::North: return @"north";
        case Direction::South: return @"south";
        case Direction::East: return @"east";
        case Direction::West: return @"west";
        case Direction::NorthWest: return @"north-west";
        case Direction::NorthEast: return @"north-east";
        case Direction::SouthEast: return @"south-east";
        case Direction::SouthWest: return @"south-west";

        default: return @"south";
    }
}

+ (instancetype)northCursor
{
    return [self cursorWithKey:@"north"];
}

+ (instancetype)southCursor
{
    return [self cursorWithKey:@"south"];
}

+ (instancetype)westCursor
{
    return [self cursorWithKey:@"west"];
}

+ (instancetype)eastCursor
{
    return [self cursorWithKey:@"east"];
}

+ (instancetype)northWestCursor
{
    return [self cursorWithKey:@"north-west"];
}

+ (instancetype)southWestCursor
{
    return [self cursorWithKey:@"south-west"];
}

+ (instancetype)northEastCursor
{
    return [self cursorWithKey:@"north-east"];
}

+ (instancetype)southEastCursor
{
    return [self cursorWithKey:@"south-east"];
}

@end
