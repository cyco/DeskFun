//
//  ZoneView.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Zone.hpp"

@interface ZoneView : NSView
- (void)setZone:(Zone*)z;
- (void)clearZone;
@property (nonatomic) uint16_t zoneID;

@property (nonatomic) BOOL useCache;
@property (nonatomic) BOOL drawsRoof;
@property (nonatomic) BOOL drawsFloor;
@property (nonatomic) BOOL drawsObject;
@property (nonatomic) NSColor *backgroundColor;
@property (nonatomic) BOOL editable;
@property BOOL isPlacing;
@property Tile* currentTile;
@property int currentLayer;
@end
