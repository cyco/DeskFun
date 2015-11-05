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
@end
