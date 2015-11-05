//
//  ItemsController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "ItemsController.h"
#import "GameDataParser.hpp"
#import "AppDelegate.h"
#import "TileView.h"

@interface ItemsController () <NSTableViewDataSource, NSTableViewDelegate>
{
    GameDataParser *data;
}
@end

@implementation ItemsController

- (NSString*)windowNibName
{
    return @"ItemsController";
}

- (void)windowDidLoad
{
    [super windowDidLoad];

    AppDelegate *dele = (AppDelegate*)[NSApp delegate];
    data = [dele GameDataParser];
    [[self tableView] reloadData];
}

#pragma mark -
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if(!data) return 0;
    return data->getItems().size();
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"item_cell" owner:self];

    TileView    *view = [[cellView subviews] objectAtIndex:0];
    NSTextField *nameLabel       = [[cellView subviews] objectAtIndex:1];
    NSTextField *additionalLabel = [[cellView subviews] objectAtIndex:2];
    [additionalLabel setStringValue:@""];

    Tile *item = data->getItems()[row];
    NSString *string = [NSString stringWithUTF8String:item->name.c_str()];
    [nameLabel setStringValue:[NSString stringWithFormat:@"%2ld (0x%03hx) %@", row, item->index, string]];

    view.tileID = item->index;

    return cellView;
}


@end
