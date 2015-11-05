//
//  DALocationView.m
//  DeskFunUI
//
//  Copyright © 2015 ccl. All rights reserved.
//

#import "DALocationView.h"

@implementation DALocationView
- (void)awakeFromNib
{
    self.up    = NO;
    self.down  = NO;
    self.left  = NO;
    self.right = NO;

    for(NSString *key in @[@"left",@"down",@"left",@"right"])
        [self addObserver:self forKeyPath:key options:0 context:NULL];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    [self setNeedsDisplay];
}

#pragma mark -
- (BOOL)isOpaque
{
    return NO;
}

- (void)drawRect:(CGRect)dirtyRect
{
    CGRect bounds = [self bounds];
    CGRect segment;
    CGFloat segmentHeight = CGRectGetHeight(bounds)/3.0;
    CGFloat segmentWidth  = CGRectGetWidth(bounds)/3.0;

    CGContextClearRect(UIGraphicsGetCurrentContext(), bounds);

    // Left Arrow
    segment = CGRectMake(CGRectGetMinX(bounds), floorf(segmentHeight), floorf(segmentWidth), ceilf(segmentHeight));
    [self _drawArrowInSegment:segment direction:2 on:self.left];

    // Right Arrow
    segment = CGRectMake(CGRectGetMaxX(bounds)-floorf(segmentWidth), floorf(segmentHeight),
                         floorf(segmentWidth), ceilf(segmentHeight));
    [self _drawArrowInSegment:segment direction:3 on:self.right];

    // Down Arrow
    segment = CGRectMake(floorf(segmentWidth), CGRectGetMaxY(bounds),
                         ceilf(segmentWidth), floorf(segmentHeight));
    [self _drawArrowInSegment:segment direction:0 on:self.down];

    // Up Arrow
    segment = CGRectMake(floorf(segmentWidth), CGRectGetMinY(bounds)-floorf(segmentHeight),
                         ceilf(segmentWidth), floorf(segmentHeight));
    [self _drawArrowInSegment:segment direction:1 on:self.up];
}

- (void)_drawArrowInSegment:(CGRect)segment direction:(int)direction on:(BOOL)flag
{
    UIBezierPath *path;
    UIColor *darkColor   = [UIColor colorWithWhite:0.5 alpha:1.0];
    UIColor *brightColor = [UIColor whiteColor];
    UIColor *fillColor   = [UIColor greenColor];

    CGFloat h1 = 0.0;
    CGFloat d1, d2;
    BOOL vertical = NO;

    BOOL lineHighlights[7];
    switch (direction) {
        case 0: // down
            vertical = YES;
            d1 = CGRectGetWidth(segment);
            d2 = -CGRectGetHeight(segment);
            memset(lineHighlights, 0, sizeof(BOOL)*7);
            lineHighlights[1] = YES; lineHighlights[3] = YES;
            break;
        case 1: // up
            vertical = YES;
            h1 = CGRectGetHeight(segment);

            d1 = CGRectGetWidth(segment);
            d2 = CGRectGetHeight(segment);
            memset(lineHighlights, 0, sizeof(BOOL)*7);
            lineHighlights[4] = lineHighlights[3] = lineHighlights[6] = lineHighlights[2] = YES;
            break;
        case 2: // left
            d1 = CGRectGetHeight(segment);
            d2 = CGRectGetWidth(segment);
            memset(lineHighlights, 0, sizeof(BOOL)*7);
            lineHighlights[4] = lineHighlights[3] = lineHighlights[6] = lineHighlights[2] = YES;
            break;
        case 3: // right
            d1 = CGRectGetHeight(segment);
            d2 = -CGRectGetWidth(segment);
            h1 = CGRectGetWidth(segment);
            memset(lineHighlights, 0, sizeof(BOOL)*7);
            lineHighlights[1] = lineHighlights[3] = true;
            break;
        default:
            h1 = d1 = d2 = 0.0;
            break;
    }

    // CGRectGetHeight(segment)-
#define MakePoint(_C1_, _C2_) (CGPoint){.x = (vertical ? _C1_ : _C2_), .y=(vertical ? _C2_ : _C1_)}
#define AddPoints(_P1_, _P2_) (CGPoint){.x = _P1_.x + _P2_.x, .y = (_P1_.y + _P2_.y)}

    // generic description of arrow
    CGFloat headStart = 2/3.0; // 1.0 and 5/6.0 also look fancy
    CGPoint arrow[7] = { MakePoint(0,        h1 + headStart * d2),
        MakePoint(d1/2,     h1 + 0), // just the tip
        MakePoint(d1,       h1 + headStart * d2),
        MakePoint(3/4.0*d1, h1 + headStart * d2),
        MakePoint(3/4.0*d1, h1 + d2),
        MakePoint(1/4.0*d1, h1 + d2),
        MakePoint(1/4.0*d1, h1 + headStart * d2) };

    // move arrow to segment
    for(int i=0; i < 7; i++)
        arrow[i] = AddPoints(arrow[i], segment.origin);


    // fill arrow if requested
    if(flag)
    {
        [fillColor setFill];

        path = [UIBezierPath bezierPath];
        [path moveToPoint:arrow[6]];
        for(int i=0; i < 8; i++)
            [path addLineToPoint:arrow[i%7]];

        [path fill];
    }

    for(int i=0; i < 7; i++)
    {
        [lineHighlights[i] ? brightColor : darkColor setStroke];
        path = [UIBezierPath bezierPath];
        [path moveToPoint:arrow[i%7]];
        [path addLineToPoint:arrow[(i+1)%7]];
        [path stroke];
    }
}
@end
