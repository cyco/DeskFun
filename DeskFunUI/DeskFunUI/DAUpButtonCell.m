//
//  DAUpButtonCell.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DAUpButtonCell.h"

@implementation DAUpButtonCell

- (NSBezierPath*)imagePathWithGlyphRect:(NSRect)rect
{
    NSBezierPath *path = [NSBezierPath bezierPath];

    [path moveToPoint:NSMakePoint(NSMinX(rect), NSMaxY(rect))];
    [path lineToPoint:NSMakePoint(NSMaxX(rect), NSMaxY(rect))];
    [path lineToPoint:NSMakePoint(NSMidX(rect), NSMinY(rect))];

    return path;
}
@end
