//
//  SoundsController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "SoundsController.h"
#import "GameDataParser.hpp"
#import "AppDelegate.h"
@interface SoundsController ()
{
    GameDataParser *data;
}
@end

@implementation SoundsController

- (NSString*)windowNibName
{
    return @"SoundsController";
}

- (void)windowDidLoad
{
    [super windowDidLoad];

    AppDelegate *dele = (AppDelegate*)[NSApp delegate];
    data = [dele GameDataParser];

    [[self tableView] reloadData];
}
#pragma mark -
- (IBAction)playSound:(id)sender
{
    NSButton *button = sender;

    NSTableView *tableView = [self tableView];
    NSView        *rowView = [button superview];
    const NSRect buttonFrameInRow = [button frame];
    const NSRect     buttonFrameInView = [tableView convertRect:buttonFrameInRow fromView:rowView];
    const NSInteger row = [tableView rowAtPoint:buttonFrameInView.origin];
    if(row != -1 && row < data->_sounds.size())
    {
        NSString *soundFile = [NSString stringWithFormat:@"%s", data->_sounds[row]->getFileName().c_str()];
        if([soundFile length] == 0) return;

        NSURL *url = [[[NSApp delegate] dataDirectoryURL] URLByAppendingPathComponent:soundFile];
        NSSound *sound = [[NSSound alloc] initWithContentsOfURL:url byReference:YES];
        [sound setName:soundFile];
        [sound play];
    }
}

#pragma mark -
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if(!data) return 0;
    return data->_sounds.size();
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"sound_cell" owner:self];

    NSTextField *nameLabel       = [cellView viewWithTag:1];
    NSTextField *additionalLabel = [cellView viewWithTag:2];
    [additionalLabel setStringValue:@""];
    [nameLabel setStringValue:[NSString stringWithUTF8String:data->_sounds[row]->getFileName().c_str()]];
    [additionalLabel setStringValue:[NSString stringWithFormat:@"0x%02lx", row]];

    NSButton *button = [cellView viewWithTag:3];
    [[button image] setTemplate:YES];
    [[button alternateImage] setTemplate:YES];
    [[button cell] setHighlightsBy:NSPushInCellMask];

    return cellView;
}
@end
