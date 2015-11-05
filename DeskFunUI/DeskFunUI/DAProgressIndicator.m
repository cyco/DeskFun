//
//  DAProgressIndicator.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DAProgressIndicator.h"

@implementation DAProgressIndicator

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self _performSetup];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self _performSetup];
    }
    return self;
}

- (void)_performSetup
{
    [self setBezelWidth:1.0];
    [self setIndicatorColor:[NSColor colorWithDeviceRed:10/255.0 green:36/255.0 blue:106/255.0 alpha:1.0]];

    NSArray *redrawKeys = @[@"minValue", @"maxValue", @"doubleValue", @"indeterminate", @"divided", @"indicatorColor"];
    [redrawKeys enumerateObjectsUsingBlock:^(NSString *key, NSUInteger idx, BOOL *stop) {
        [self addObserver:self forKeyPath:key options:0 context:nil];
    }];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if([keyPath isEqualToString:@"doubleValue"])
    {
        if([self doubleValue] > [self maxValue])
            _doubleValue = _maxValue;
    }
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    const NSRect bounds = [self bounds];
    const CGFloat inset = 2.0;
    NSRect progressRect = NSInsetRect(bounds, inset, inset);

    CGFloat progress = ([self doubleValue]-[self minValue])/([self maxValue]-[self minValue]);
    progressRect.size.width *= progress;
    [[self indicatorColor] setFill];


    if(_segmented){
        NSRectFill(progressRect);
        return;
    }

    const CGFloat segmentWidth = 6.0;
    const CGFloat segmentSpace = 2.0;

    NSRect segmentRect = progressRect;
    segmentRect.size.width = segmentWidth;

    while(NSMaxX(segmentRect) < NSMaxX(progressRect))
    {
        NSRectFill(segmentRect);
        segmentRect.origin.x += segmentWidth + segmentSpace;
    }

    if(_adjustEverySegment || NSMaxX(progressRect) >= NSWidth(bounds)-inset) {
        segmentRect.size.width = NSMaxX(progressRect) - segmentRect.origin.x;
        NSRectFill(segmentRect);
    }
}

@end
