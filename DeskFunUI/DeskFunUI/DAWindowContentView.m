//
//  DAWindowContentView.m
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAWindowContentView.h"
#import "DAWindowTheme.h"
@implementation DAWindowContentView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

- (BOOL)isOpaque
{
    return [[[DAWindowTheme sharedTheme] backgroundColor] alphaComponent] == 1.0;
}

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];
    [[[DAWindowTheme sharedTheme] backgroundColor] setFill];
    NSRectFill(dirtyRect);
}
@end
