//
//  DAButtonCell.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DAGlyphButtonCell : NSButtonCell
- (NSBezierPath*)imagePathWithGlyphRect:(NSRect)rect;
@end
