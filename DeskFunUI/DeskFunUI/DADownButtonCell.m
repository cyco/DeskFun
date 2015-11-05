//
//  DADownButtonCell.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DADownButtonCell.h"

@implementation DADownButtonCell

- (NSBezierPath*)imagePathWithGlyphRect:(NSRect)rect
{
    NSBezierPath *path = [NSBezierPath bezierPath];

    [path moveToPoint:NSMakePoint(NSMinX(rect), NSMinY(rect))];
    [path lineToPoint:NSMakePoint(NSMaxX(rect), NSMinY(rect))];
    [path lineToPoint:NSMakePoint(NSMidX(rect), NSMaxY(rect))];

    return path;
}

@end
