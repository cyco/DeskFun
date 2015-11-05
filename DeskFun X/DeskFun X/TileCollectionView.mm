//
//  TileCollectionView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//
#import "TileCollectionView.h"

#import "AppDelegate.h"

#include <vector>
#include "Tile.hpp"
@interface TileCollectionView ()
{
    std::vector<Tile> tiles;
}
@property uint32_t filter;
@end

@implementation TileCollectionView

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
}

- (IBAction)checkBit:(id)sender
{
    // very fragile way to get the selected bit
    NSUInteger index = [[[[self enclosingScrollView] superview] subviews] indexOfObject:sender];

    uint32_t mask = 1<<index;
    if([sender state] == NSOnState)
        _filter &= ~mask;
    else
        _filter |= mask;

    [self _filterUpdated];
}

- (void)_filterUpdated
{
}
@end
