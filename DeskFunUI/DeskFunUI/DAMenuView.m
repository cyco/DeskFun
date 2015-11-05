//
//  DAMenuView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DAMenuView.h"

const static CGFloat ItemHeight = 23.0;
__unused const static CGFloat SeparatorHeight = 14.0;
@interface DAMenuView ()
@end

@implementation DAMenuView
@synthesize menu=_menu;

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    NSMenu   *menu = [self menu];
    NSArray *items = [menu itemArray];

    const NSRect bounds = [self bounds];
    CGFloat y     = NSHeight(bounds)-5.0;
    CGFloat width = NSWidth(bounds)-10.0;
    CGFloat x     = 5.0;

    for(NSMenuItem *item in items)
    {
        y -= ItemHeight;
        NSString *title = [item title];
        [title drawInRect:NSMakeRect(x, y, width, ItemHeight) withAttributes:nil];
    }
}


- (void)setMenu:(NSMenu*)menu
{
    CGFloat height = [menu numberOfItems] * ItemHeight;
    CGFloat width = 140.0;

    // TODO: check items
    _menu = menu;

    [self setSwapColors:YES];
    [self setBezelWidth:1.0];

    NSWindow *window = [self window];
    NSRect frame = [window frame];
    frame.size = (NSSize){.width=width,.height=height};
    [window setFrame:frame display:YES];
}

@end
