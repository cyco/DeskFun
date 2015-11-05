//
//  CharsController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "CharsController.h"
#import "AppDelegate.h"
#import "GameDataParser.hpp"
@interface CharsController () <NSTableViewDataSource, NSTableViewDelegate>
{
    GameDataParser *data;
    int currentFrame;
    Char *c;
}
@end

@implementation CharsController

- (NSString*)windowNibName
{
    return @"CharsController";
}

- (void)windowDidLoad {
    [super windowDidLoad];

    AppDelegate *dele = (AppDelegate *)[NSApp delegate];
    data = [dele GameDataParser];

    currentFrame = 0;

    [[self tableView] reloadData];
}


- (void)showChar:(NSInteger)row
{
    c = data->_chars[row];
    if(!c) return;

    currentFrame = 0;
    [self updateFrames];

    // Frames:
    NSMutableString *aux = [NSMutableString string];
    for(int i=0; i < 2; i++)
    {
        [aux appendFormat:@"%02x ", c->getAux()[i]];
    }
    [[self auxField] setStringValue:aux];

    NSMutableString *wpn = [NSMutableString string];
    for(int i=0; i < 4; i++)
    {
        [wpn appendFormat:@"%02x ", c->getWeapon()[i]];
    }
    [[self weaponField] setStringValue:wpn];


    NSMutableString *specs = [NSMutableString stringWithFormat:@"Specs length: %ld\n", c->getSpecsLength()];

    uint16_t type = c->getSpecs()[16] | c->getSpecs()[17] << 8;
    if(type == 0x0001)
        [specs appendFormat:@"%8s: %8@", "type", @"Hero"];
    else if(type == 0x0002)
        [specs appendFormat:@"%8s: %8@", "type", @"Enemy"];
    else if(type == 0x0004)
        [specs appendFormat:@"%8s: %8@", "type", @"Weapon"];
    else
        [specs appendFormat:@"%8s: %8@", "type", @"Unknown"];
    [specs appendString:@"\n"];

    // 4 bytes \0, then:
    for(int i=4; i < c->getSpecsLength(); i++)
    {
        if(i != 16 && i != 17)
            [specs appendFormat:@"%02x ", c->getSpecs()[i]];
    }
    [[self specsField] setString:specs];
}

- (void)updateFrames
{
    currentFrame %= 3;

    CharFrame f = c->frames[currentFrame];

    self.left.tileID    = f.left;
    self.left_ex.tileID = f.extension_left;

    self.right.tileID    = f.right;
    self.right_ex.tileID = f.extension_right;

    self.up.tileID    = f.up;
    self.up_ex.tileID = f.extension_up;

    self.down.tileID = f.down;
    self.down_ex.tileID = f.extension_down;
}

- (IBAction)step:(id)sender
{
    currentFrame ++;
    [self updateFrames];
}
#pragma mark -
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if(!data) return 0;
    return data->_chars.size();
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    return [NSString stringWithFormat:@"0x%02lx: %s", (long)row, data->_chars[row]->getName().c_str()];
}

- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row
{
    [self showChar:row];
    return YES;
}

@end
