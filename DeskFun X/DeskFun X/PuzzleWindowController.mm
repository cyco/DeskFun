//
//  PuzzleWindowController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "PuzzleWindowController.h"
#import "GameDataParser.hpp"
#import "AppDelegate.h"
#import "Puzzle.hpp"
#import "Tile.hpp"
#import "TileView.h"
@interface PuzzleWindowController () <NSTableViewDataSource, NSTableViewDelegate>
{
    GameDataParser *data;
}
@end

@implementation PuzzleWindowController

- (NSString*)windowNibName
{
    return @"PuzzleWindowController";
}

- (void)windowDidLoad {
    [super windowDidLoad];

    AppDelegate *dele = (AppDelegate*)[NSApp delegate];
    data = [dele GameDataParser];

    [[self tableView] setDataSource:self];
    [[self tableView] setDelegate:self];
    [[self tableView] reloadData];
}

#pragma mark - Table View DataSource
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return data->_puzzles.size();
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"puzzle_cell" owner:self];
    NSTextField *label = nil;

    Puzzle *p = data->_puzzles[row];

    label = [[cellView subviews] objectAtIndex:9];
    [label setStringValue:[NSString stringWithFormat:@"0x%02x", (int)row]];

    TileView    *view1 = [[cellView subviews] objectAtIndex:0];
    [view1 setTileID:p->getQuestItem(0) ? p->getQuestItem(0)->index : 0xFFFF];
    label = [[cellView subviews] objectAtIndex:7];
    [label setStringValue:[NSString stringWithFormat:@"0x%03hx", p->getQuestItem(0)->index]];
    TileView    *view2 = [[cellView subviews] objectAtIndex:1];
    [view2 setTileID:p->getQuestItem(1) ? p->getQuestItem(1)->index : 0xFFFF];
    label = [[cellView subviews] objectAtIndex:8];
    [label setStringValue:[NSString stringWithFormat:@"0x%03hx", p->getQuestItem(1)->index]];

    for(int i=0; i < 5; i++)
    {
        NSTextField *label = [[cellView subviews] objectAtIndex:i+2];

        const char *string = p->getText(i).c_str();
        NSString *labelString = strlen(string) ?  [NSString stringWithFormat:@"%d. %s", i+1,  string] : @"";
        [label setStringValue:labelString];
    }

    label = [[cellView subviews] objectAtIndex:6];

    NSString *typeString = p->getType() == PuzzleTypeEnd ? @"End" : [NSString stringWithFormat:@"%02x", p->getType()];
    NSString *labelString = [NSString stringWithFormat:@"%s %3@ %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                             data->_isYodaVersion ? "" : p->getName().c_str(),
                             typeString,
                             p->_unknown[0], p->_unknown[1], p->_unknown[2], p->_unknown[3], p->_unknown[4],
                             p->_unknown[5], p->_unknown[6], p->_unknown[7], p->_unknown[8], p->_unknown[9]];
    [label setStringValue:labelString];

    return cellView;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    NSInteger row = self.tableView.selectedRow;
    if(row == -1) return;
    
    Puzzle *p = data->_puzzles[row];
    
    for(int i=0; i < 5; i++)
    {
        const char *string = p->getText(i).c_str();
        NSString *labelString = strlen(string) ?  [NSString stringWithFormat:@"%d. %s", i+1,  string] : @"";
        printf("%s\n", [labelString cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

@end
