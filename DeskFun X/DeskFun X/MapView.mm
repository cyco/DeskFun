//
//  MapView.m
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#import "MapView.h"
#include "MapGenerator.hpp"

@implementation MapView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    [[NSColor grayColor] setFill];
    NSRectFill(self.bounds);

    if(!self.map) return;
    
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            uint16_t value = self.map[idx];
            NSBitmapImageRep *image = [self imageForValue:(MapType)value];
            [image drawInRect:NSMakeRect(x*28, NSHeight(self.bounds) - y*28 - 28, 28, 28) fromRect:NSMakeRect(0, 4, 28, 28) operation:NSCompositeCopy fraction:1.0 respectFlipped:true hints:nil];
        }
    }
}

- (NSBitmapImageRep*)imageForValue:(MapType)value {
    int idx = 0;
    switch(value) {
        case MapType_TRAVEL_START:
        case MapType_TRAVEL_END: idx = 2; break;
        case MapType_PUZZLE: idx = 0; break;
        case MapType_ISLAND:
        case MapType_EMPTY:
        case MapType_PUZZLE_CANDIDATE: idx = 15; break;
        case MapType_BLOCK_EAST: idx = 6; break;
        case MapType_BLOCK_WEST: idx = 10; break;
        case MapType_BLOCK_NORTH: idx = 4; break;
        case MapType_BLOCK_SOUTH: idx = 8; break;
        case MapType_SPACEPORT: idx = 12; break;
        case MapType_KEPT_FREE:
        case MapType_INVALID:
        case MapType_NONE: idx = 19; break;
    }
    
    return [[self tileImages] objectAtIndex:idx];
}

@end
