//
//  TileLayer.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "Tile.hpp"
@interface TileLayer : CAOpenGLLayer
@property (nonatomic, assign) Tile *tile;
- (void)tick;
@end
