//
//  DADisabledScrollView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DADisabledScrollView.h"

@implementation DADisabledScrollView
- (void)scrollWheel:(NSEvent *)theEvent
{
    [[self nextResponder] scrollWheel:theEvent];
}
@end
