//
//  DACircleButtonCell.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DACircleButtonCell.h"

@implementation DACircleButtonCell

- (NSBezierPath*)imagePathWithGlyphRect:(NSRect)rect
{
    return [NSBezierPath bezierPathWithOvalInRect:rect];
}

@end
