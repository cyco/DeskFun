//
//  DACursor.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DACursor : NSCursor
+ (NSCursor*)defaultCursor;

+ (instancetype)cursorWithDirection:(float)direction;

+ (instancetype)blockingCursor;
+ (instancetype)northCursor;
+ (instancetype)southCursor;
+ (instancetype)westCursor;
+ (instancetype)eastCursor;
+ (instancetype)northWestCursor;
+ (instancetype)southWestCursor;
+ (instancetype)northEastCursor;
+ (instancetype)southEastCursor;
@end
