//
//  DASpeechBubbleWindow.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DASpeechBubbleWindow.h"
#import "DASpeechBubbleView.h"

#import <DeskFunUI/DAUpButtonCell.h>
#import <DeskFunUI/DADownButtonCell.h>
#import <DeskFunUI/DACircleButtonCell.h>

@implementation DASpeechBubbleWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
    aStyle = [self _defaultStyleMask];
    self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag];
    if(self)
    {
        [self _performCommonSetup];
    }
    return self;
}

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag screen:(NSScreen *)screen
{
    aStyle = [self _defaultStyleMask];
    self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag screen:screen];
    if(self)
    {
        [self _performCommonSetup];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if(self)
    {
        [self _performCommonSetup];
    }
    return self;
}

+ (NSRect)frameRectForContentRect:(NSRect)cRect styleMask:(NSUInteger)aStyle
{
    return NSInsetRect(cRect, -15, -15);
}

+ (NSRect)contentRectForFrameRect:(NSRect)fRect styleMask:(NSUInteger)aStyle
{
    return NSInsetRect(fRect, 15, 15);
}

- (NSInteger)_defaultStyleMask
{
    return NSBorderlessWindowMask;
}

- (void)_performCommonSetup
{
    [self setOpaque:NO];
    [self setHasShadow:NO];
    [self setBackgroundColor:[NSColor clearColor]];

    [self setMovableByWindowBackground:YES];

    DASpeechBubbleView *contentView = [[DASpeechBubbleView alloc] initWithFrame:NSMakeRect(0, 0, 140, 140)];
    [contentView setType:BottomLeft];

    NSRect frame = [self frame];
    frame.origin = NSMakePoint(0, 0);
    [contentView setFrame:frame];

    [self _createButtonNumber:0 withCell:[DACircleButtonCell class] inView:contentView];
    [self _createButtonNumber:1 withCell:[DADownButtonCell class] inView:contentView];
    [self _createButtonNumber:2 withCell:[DAUpButtonCell class] inView:contentView];

    [self setContentView:contentView];
}

- (NSButton*)_createButtonNumber:(NSInteger)num withCell:(Class)CellClass inView:(NSView*)view
{
    const CGFloat height = 16.0, width = 16.0;
    const NSRect viewFrame   = NSInsetRect([view frame], 15, 15);
    const NSRect buttonFrame = NSMakeRect(NSMaxX(viewFrame)-16-7, NSMinY(viewFrame)+6+num*height+num, width, height);

    NSButton *button = [[NSButton alloc] initWithFrame:buttonFrame];
    [button setAutoresizingMask:NSViewMinXMargin|NSViewMaxYMargin];
    [button setButtonType:NSMomentaryPushInButton];
    [button setImagePosition:NSNoImage];
    [button setCell:[[CellClass alloc] initImageCell:[NSImage imageNamed:NSImageNameActionTemplate]]];

    [view addSubview:button];

    return button;
}
@end
