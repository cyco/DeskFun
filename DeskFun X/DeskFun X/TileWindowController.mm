//
//  TileWindowController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "TileWindowController.h"

#import "AppDelegate.h"
#include "Tile.hpp"
#include "GameDataParser.hpp"

@interface TileWindowController ()
@end

@implementation TileWindowController

- (void)windowDidLoad
{
    [super windowDidLoad];
}

- (IBAction)changeTile:(id)sender
{
    NSString *string = [sender stringValue];
    unsigned int index  = 0;

    BOOL isHex = NO;
    if([string rangeOfString:@"0x"].location == 0) {
        isHex = YES;
        NSScanner *scanner = [NSScanner scannerWithString:string];
        [scanner setScanLocation:2];
        [scanner scanHexInt:&index];
    } else {
        index = [string intValue];
    }

    index %= GameContext::CurrentContext()->getTileCount();
    Tile *t = GameContext::CurrentContext()->getTile(index);

    [[self tView] setTile:t];

    uint32_t specs = t->debugGetSpecs();
    NSMutableArray *flags = [NSMutableArray array];

    if(t->name.length())
    {
        [flags addObject:[NSString stringWithFormat:@"%s", t->name.c_str()]];
    }

#define TestBit(_BIT_, _NAME_) [flags addObject:[NSString stringWithFormat:@"%10s: %5@", _NAME_, (specs & (1<<_BIT_)) ? @"YES" : @"NO"]]

    if(t->debugIsAnimated) [flags addObject:@"animated"];
    TestBit(0, "transparent");
    TestBit(1, "floor");
    TestBit(2, "object");
    TestBit(3, "draggable");
    TestBit(4, "roof");
    TestBit(5, "locator");
    TestBit(6, "weapon");
    TestBit(7, "item");
    TestBit(8, "character");
    [flags addObject:@""];
    if(specs & (1<<6)) {
        TestBit(16, "blaster (l)");
        TestBit(17, "blaster (h)");
        TestBit(18, "lightsaber");
        TestBit(19, "The Force");
    } else if(specs & (1<<7)) {
        TestBit(16, "keycard");
        TestBit(17, "puzzle tool");
        TestBit(18, "puzzle goal?");
        TestBit(19, "puzzle value");
        TestBit(20, "locator");
        TestBit(22, "health");
    } else if(specs & (1<<8)) {
        TestBit(16, "hero");
        TestBit(17, "enemy");
        TestBit(18, "npc");
    } else if(specs & (1<<5)) {
        TestBit(17, "home");
        TestBit(18, "puz, unsolved");
        TestBit(19, "puz, solved");
        TestBit(20, "port, unsolved");
        TestBit(21, "port, solved");
        TestBit(22, "N unso");
        TestBit(23, "S unso");
        TestBit(24, "E unso");
        TestBit(25, "W unso");

        TestBit(26, "N solved");
        TestBit(27, "S solved");
        TestBit(28, "E solved");
        TestBit(29, "W solved");

        TestBit(30, "Goal");
    } else {
        TestBit(16, "Trigger");
    }

    [[self sView] setString:[flags componentsJoinedByString:@"\n"]];

    if(isHex) {
        [sender setStringValue:[NSString stringWithFormat:@"0x%0x", index]];
    } else {
        [sender setStringValue:[NSString stringWithFormat:@"%0u", index]];
    }
}

- (NSString*)windowNibName
{
    return @"TileWindowController";
}
@end
