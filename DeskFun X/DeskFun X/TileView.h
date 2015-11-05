//
//  TileView.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "Tile.hpp"

@interface TileView : NSView
@property (nonatomic, assign) Tile *tile;
@property (nonatomic) uint16_t tileID;
@end

