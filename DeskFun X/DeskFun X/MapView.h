//
//  MapView.h
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MapView : NSView
@property uint16_t *map;
@property (strong) NSArray *tileImages;
@end
