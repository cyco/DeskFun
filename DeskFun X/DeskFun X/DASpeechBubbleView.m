//
//  DASpeechBubbleView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DASpeechBubbleView.h"

@implementation DASpeechBubbleView

- (instancetype)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self _performCommonSetup];
    }
    return self;
}
- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self _performCommonSetup];
    }
    return self;
}

- (void)_performCommonSetup
{
    _lineColor = [NSColor blackColor];
    _backgroundColor = [NSColor whiteColor];
}

#pragma mark - Sizes

- (NSSize)arrowSize
{
    return NSMakeSize(15.0, 15.0);
}

- (NSSize)cornerRadius
{
    return NSMakeSize(7.0, 7.0);
}

#pragma mark - View Drawing

- (BOOL)isOpaque
{
    return NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    [[NSColor clearColor] setFill];
    NSRectFill(bounds);

    [_backgroundColor setFill];
    [_lineColor setStroke];

    const NSRect frame = NSInsetRect(bounds, 2.0, 2.0); // leave some room for the line

    NSBezierPath *path = [self _boundaryPathInFrame:frame];
    [path setLineWidth:1.5];
    [path stroke];
    [path fill];
}

- (NSBezierPath*)_boundaryPathInFrame:(NSRect)rect
{
    const NSSize arrowSize = [self arrowSize];
    const NSSize cornerRadius = [self cornerRadius];
    const NSRect frame = NSInsetRect(rect, arrowSize.width, arrowSize.height);
    NSPoint *points = [self _arrowCoordinatesForType:_type inRect:frame];

    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:frame xRadius:cornerRadius.width yRadius:cornerRadius.height];

    [path moveToPoint:points[0]];
    [path lineToPoint:points[1]];
    [path lineToPoint:points[2]];

    free(points);

    return path;
}

- (NSPoint*)_arrowCoordinatesForType:(ArrowType)type inRect:(NSRect)frame
{
    const NSSize arrowSize = [self arrowSize];
    NSPoint *points = (NSPoint*)calloc(sizeof(NSPoint), 3);
    switch (_type) {
        case BottomLeft:
            points[0] = (NSPoint){NSMidX(frame), NSMinY(frame)};
            points[1] = (NSPoint){NSMidX(frame), NSMinY(frame) - arrowSize.height};
            points[2] = (NSPoint){NSMidX(frame) + arrowSize.width, NSMinY(frame)};
            break;
        case BottomRight:
            points[0] = (NSPoint){NSMidX(frame), NSMinY(frame)};
            points[1] = (NSPoint){NSMidX(frame), NSMinY(frame) - arrowSize.height};
            points[2] = (NSPoint){NSMidX(frame) - arrowSize.width, NSMinY(frame)};
            break;
        case TopLeft:
            points[0] = (NSPoint){NSMidX(frame), NSMaxY(frame)};
            points[1] = (NSPoint){NSMidX(frame), NSMaxY(frame) + arrowSize.height};
            points[2] = (NSPoint){NSMidX(frame) + arrowSize.width, NSMaxY(frame)};
            break;
        case TopRight:
            points[0] = (NSPoint){NSMidX(frame), NSMaxY(frame)};
            points[1] = (NSPoint){NSMidX(frame), NSMaxY(frame) + arrowSize.height};
            points[2] = (NSPoint){NSMidX(frame) - arrowSize.width, NSMaxY(frame)};
            break;
        case LeftBottom:
            points[0] = (NSPoint){NSMinX(frame), NSMidY(frame)};
            points[1] = (NSPoint){NSMinX(frame) - arrowSize.width, NSMidY(frame)};
            points[2] = (NSPoint){NSMinX(frame), NSMidY(frame) + arrowSize.height};
            break;
        case LeftTop:
            points[0] = (NSPoint){NSMinX(frame), NSMidY(frame)};
            points[1] = (NSPoint){NSMinX(frame) - arrowSize.width, NSMidY(frame)};
            points[2] = (NSPoint){NSMinX(frame), NSMidY(frame) - arrowSize.height};
            break;
        case RightBottom:
            points[0] = (NSPoint){NSMaxX(frame), NSMidY(frame)};
            points[1] = (NSPoint){NSMaxX(frame) + arrowSize.width, NSMidY(frame)};
            points[2] = (NSPoint){NSMaxX(frame), NSMidY(frame) + arrowSize.height};
            break;
        case RightTop:
            points[0] = (NSPoint){NSMaxX(frame), NSMidY(frame)};
            points[1] = (NSPoint){NSMaxX(frame) + arrowSize.width, NSMidY(frame)};
            points[2] = (NSPoint){NSMaxX(frame), NSMidY(frame) - arrowSize.height};
            break;
        default:
            break;
    }
    return points;
}

@end
