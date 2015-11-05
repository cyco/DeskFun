//
//  DAWindowMenu.m
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAMenuBarView.h"
#import "DAWindowTheme.h"
#import "DAMenuView.h"

@interface DAMenuBarView ()
@property CGFloat *widths;
@property NSInteger highlightedIndex;
@property BOOL clicked;

@property NSWindow *menuWindow;
@property DAMenuView *menuView;
@end

const CGFloat leftBorder  = 6.0;
const CGFloat rightBorder = 5.0;
@implementation DAMenuBarView
@synthesize menu=_menu;

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self _commonInit];
    }
    return self;
}

- (void)_commonInit
{
    _widths = NULL;
    _highlightedIndex = NSNotFound;

    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSZeroRect styleMask:NSBorderlessWindowMask backing:0 defer:NO];
    DAMenuView *menuView = [[DAMenuView alloc] initWithFrame:NSZeroRect];
    [menuView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
    [window setContentView:menuView];
    [window setReleasedWhenClosed:NO];
    [self setMenuView:menuView];
    [self setMenuWindow:window];
}

- (void)dealloc
{
    free(_widths);
}
#pragma mark -
- (NSFont*)font
{
    return [NSFont systemFontOfSize:12.0];
//    return [NSFont fontWithName:@"Microsoft Sans Serif" size:12.0];
}
#pragma mark -
- (void)setMenu:(NSMenu *)menu
{
    _menu = menu;

    [self hideSubmenu];

    if(_widths) free(_widths);

    NSInteger count = [menu numberOfItems];
    _widths = calloc(count, sizeof(CGFloat));
    for(NSInteger i=0; i < count; i++)
    {
        NSMenuItem *item = [menu itemAtIndex:i];
        NSString  *title = [item title];
        NSFont     *font = [self font];

        NSAttributedString *attributedTitle = [[NSAttributedString alloc] initWithString:title attributes:@{NSFontAttributeName:font}];
        _widths[i] = leftBorder+rightBorder+[attributedTitle size].width;
    }

    [self updateTrackingAreas];
    [self setNeedsDisplay:YES];
}

- (void)showSubmenu:(NSMenu*)menu at:(NSPoint)p
{
    DAMenuView *menuView = [self menuView];
    NSWindow *gameWindow = [self window];
    NSWindow *menuWindow = [self menuWindow];

    NSPoint gameWindowOrigin = [gameWindow frame].origin;
    NSPoint o = (NSPoint){.x=gameWindowOrigin.x+p.x,.y=gameWindowOrigin.y+p.y};

    NSRect r = [menuWindow frame];

    r.origin.x = o.x;
    r.origin.y = o.y - NSHeight(r);

    [self backingAlignedRect:r options:NSAlignAllEdgesOutward];
    [menuWindow setFrame:r display:YES];

    if([menuWindow parentWindow] == nil)
        [gameWindow addChildWindow:menuWindow ordered:NSWindowAbove];

    [menuView setMenu:menu];
}

- (void)hideSubmenu
{
    NSWindow *menuWindow = [self menuWindow];
    [menuWindow close];
}
#pragma mark - Interaction
- (void)updateTrackingAreas
{
    [[[self trackingAreas] copy] enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        [self removeTrackingArea:obj];
    }];

    const NSRect bounds = [self bounds];
    const NSInteger itemCount = [_menu numberOfItems];
    const NSTrackingAreaOptions options = NSTrackingMouseMoved|NSTrackingMouseEnteredAndExited|NSTrackingActiveAlways;
    CGFloat x = 0;
    for(NSInteger i=0; i < itemCount; i++)
    {
        NSRect rect = NSMakeRect(x, 0, _widths[i], NSHeight(bounds));
        NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:rect options:options owner:self userInfo:nil];
        [self addTrackingArea:area];
        x += _widths[i];
    }
}


- (void)mouseDown:(NSEvent*)theEvent
{
    NSPoint locationInWindow  = [theEvent locationInWindow];
    NSPoint locationInView    = [self convertPoint:locationInWindow fromView:nil];
    const NSInteger itemCount = [_menu numberOfItems];
    CGFloat totalWidth = 0;
    for(int i=0; i < itemCount; i++) totalWidth+=_widths[i];

    if(!_clicked && locationInView.x <= totalWidth)
        _clicked = !_clicked;
    else _clicked = NO;

    [self mouseMoved:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
    [self mouseMoved:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSPoint locationInWindow  = [theEvent locationInWindow];
    NSPoint locationInView    = [self convertPoint:locationInWindow fromView:nil];
    const NSInteger itemCount = [_menu numberOfItems];
    CGFloat x = 0.0;
    _highlightedIndex = NSNotFound;
    for(NSInteger i=0; i < itemCount; i++)
    {
        CGFloat location = locationInView.x;
        if(x <= location && location <= x+_widths[i])
        {
            _highlightedIndex = i;
            break;
        }
        x+=_widths[i];
    }

    if(_highlightedIndex == NSNotFound && _clicked)
    {
        if(locationInView.x < 0) _highlightedIndex = 0;
        else _highlightedIndex = itemCount-1;
    }

    if(_highlightedIndex == NSNotFound || !_clicked)
    {
        [self hideSubmenu];
    }
    else if(_highlightedIndex != NSNotFound  && _clicked)
    {
        NSMenuItem *item = [[self menu] itemAtIndex:_highlightedIndex];
        NSMenu  *submenu = [item submenu];

        CGFloat x = 0;
        for(int i=0; i < _highlightedIndex; i++) x += _widths[i];

        NSPoint p = [self convertPoint:(NSPoint){.x=x,.y=0} toView:[self superview]];
        [self showSubmenu:submenu at:p];
    }

    [self setNeedsDisplay:YES];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    [self mouseMoved:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
    if(!_clicked)
        _highlightedIndex = NSNotFound;
    else
        [self mouseMoved:theEvent];
    [self setNeedsDisplay:YES];
}
#pragma mark - Drawing
- (void)drawRect:(NSRect)dirtyRect
{
    // Bezel Drawing
    DAWindowTheme *theme = [DAWindowTheme sharedTheme];
    const CGFloat bezelWidth = 1.0;
    BOOL swapColors = !_clicked;

    const NSRect bounds = [self bounds];
    const NSInteger itemCount = [_menu numberOfItems];

    CGFloat x = 0.0;
    for(NSInteger i=0; i < itemCount; i++)
    {
        NSMenuItem *item   = [[self menu] itemAtIndex:i];
        NSMenu   *submenu  = [item submenu];
        NSString   *title  = [item title];
        NSFont     *font   = [self font];
        NSString *mnemonic = [submenu title];

        NSRect rect = NSMakeRect(x, 0, _widths[i], NSHeight(bounds));
        rect = [self backingAlignedRect:rect options:NSAlignAllEdgesOutward];

        if(i == _highlightedIndex)
        {
            CGFloat minY = NSMinY(rect);
            CGFloat maxY = NSMaxY(rect);
            CGFloat minX = NSMinX(rect);
            CGFloat maxX = NSMaxX(rect);

            NSBezierPath *path = [NSBezierPath bezierPath];
            [path moveToPoint:NSMakePoint(minX, minY)];
            [path lineToPoint:NSMakePoint(minX, maxY)];
            [path lineToPoint:NSMakePoint(maxX, maxY)];

            [path lineToPoint:NSMakePoint(maxX-bezelWidth, maxY-bezelWidth)];
            [path lineToPoint:NSMakePoint(minX+bezelWidth, maxY-bezelWidth)];
            [path lineToPoint:NSMakePoint(minX+bezelWidth, minY+bezelWidth)];
            [path lineToPoint:NSMakePoint(minX, minY)];

            [swapColors ? [theme bezelColorLight] : [theme bezelColorDark] set];
            [path fill];

            path = [NSBezierPath bezierPath];
            [path moveToPoint:NSMakePoint(minX, minY)];
            [path lineToPoint:NSMakePoint(maxX, minY)];
            [path lineToPoint:NSMakePoint(maxX, maxY)];

            [path lineToPoint:NSMakePoint(maxX-bezelWidth, maxY-bezelWidth)];
            [path lineToPoint:NSMakePoint(maxX-bezelWidth, minY+bezelWidth)];
            [path lineToPoint:NSMakePoint(minX+bezelWidth, minY+bezelWidth)];
            [path lineToPoint:NSMakePoint(minX, minY)];
            
            [swapColors ? [theme bezelColorDark] : [theme bezelColorLight] set];
            [path fill];
        }

        rect = NSInsetRect(rect, 0, 0);
        rect.origin.x   += leftBorder;
        rect.size.width -= leftBorder+rightBorder;

        if(_clicked && _highlightedIndex == i)
        {
            rect.origin.x += 1.0;
            rect.origin.y -= 1.0;
        }

        NSRange r = [title rangeOfString:mnemonic];
        if(r.location == NSNotFound) continue;

        NSMutableAttributedString *string = [[NSMutableAttributedString alloc] initWithString:title attributes:@{NSFontAttributeName:font}];
        if(r.location != NSNotFound)
        {
            [string addAttribute:NSUnderlineStyleAttributeName value:[NSNumber numberWithInt:NSUnderlineStyleSingle] range:r];
        }
        rect.origin.y += 1.0;
        [string drawInRect:rect];

        x += _widths[i];
    }
}
@end
