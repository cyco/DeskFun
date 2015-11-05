//
//  DASpeechContoller.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DASpeechContoller.h"

@interface DASpeechContoller ()
@property (copy, nonatomic) NSString *text;
@property (strong) NSArray *lines;
@end

@implementation DASpeechContoller

+ (instancetype)showText:(NSString*)text atPoint:(NSPoint)p ofView:(NSView*)view
{
    DASpeechContoller *controller = [[DASpeechContoller alloc] initWithWindowNibName:@"DASpeechContoller"];
    [controller setText:text];

    NSWindow *gameWindow = [view window];
    NSWindow *bubbleWindow = [controller window];

    p = [view convertPoint:p toView:nil];
    p = [gameWindow convertRectToScreen:(NSRect){p, NSZeroSize}].origin;

    p.x -= [bubbleWindow frame].size.width / 2.0;
    p.y += [bubbleWindow frame].size.height/2.0;
    [bubbleWindow setFrameOrigin:p];

    [gameWindow addChildWindow:bubbleWindow ordered:NSWindowAbove];

    [controller _updateButtonState];

    return controller;
}

- (NSString*)windowNibName
{
    return @"DASpeechContoller";
}

- (void)windowDidLoad {
    [super windowDidLoad];

    [self setText:[self text]];
    [self _updateButtonState];
}

- (void)setText:(NSString *)text
{
    _text = text;

    [[self textView] setString:text];
}
#pragma mark - 
- (void)_updateButtonState
{
    BOOL isAtStart = [self _scrollingIsAtStart];
    BOOL isAtEnd   = [self _scrollingIsAtEnd];

    [[self scrollDownButton] setHidden:isAtEnd && isAtStart];
    [[self scrollUpButton] setHidden:isAtEnd && isAtStart];

    [[self scrollUpButton] setEnabled:!isAtStart];
    [[self scrollDownButton] setEnabled:!isAtEnd];

    [[self closeButton] setEnabled:isAtEnd];
}
#pragma mark - 
- (NSFont*)font
{
    return [NSFont fontWithName:@"Microsoft Sans Serif" size:12.0];
}
#pragma mark - UI Actions
- (IBAction)scrollUp:(id)sender
{
    if([self _scrollingIsAtStart]) return;

    NSView *textView = [self textView];
    NSRect rect = [textView visibleRect];
    rect.origin.y -= 12.0;
    [textView scrollRectToVisible:rect];
    [self _updateButtonState];
}

- (IBAction)scrollDown:(id)sender
{
    if([self _scrollingIsAtEnd]) return;

    NSView *textView = [self textView];
    NSRect rect = [textView visibleRect];
    rect.origin.y += 12.0;
    [textView scrollRectToVisible:rect];
    [self _updateButtonState];
}

- (IBAction)close:(id)sender
{
}

- (BOOL)_scrollingIsAtEnd
{
    NSView *textView = [self textView];
    return NSMaxY([textView visibleRect]) == NSMaxY([textView frame]);
}

- (BOOL)_scrollingIsAtStart
{
    NSView *textView = [self textView];
    return NSMinY([textView visibleRect]) == NSMinY([textView frame]);
}
@end
