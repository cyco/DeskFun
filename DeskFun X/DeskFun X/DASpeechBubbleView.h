//
//  DASpeechBubbleView.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef enum : NSUInteger {
    BottomLeft,
    BottomRight,
    TopLeft,
    TopRight,
    LeftTop,
    LeftBottom,
    RightTop,
    RightBottom
} ArrowType;

@interface DASpeechBubbleView : NSView
@property ArrowType type;
@property NSColor *backgroundColor;
@property NSColor *lineColor;
@end
