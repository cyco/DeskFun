//
//  ZoneWindowController.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "ActionWindowController.h"
@class ZoneView, HexView;

@interface ZoneWindowController : NSWindowController

@property (assign) IBOutlet NSTableView *tableView;
@property (assign) IBOutlet NSTableView *itemsTableView1;
@property (assign) IBOutlet NSTableView *itemsTableView2;
@property (assign) IBOutlet NSTableView *itemsTableView3;
@property (assign) IBOutlet NSTableView *itemsTableView4;
@property (assign) IBOutlet ZoneView *zoneView;
@property (assign) IBOutlet NSTableView *actionsView;
@property (assign) IBOutlet ActionWindowController *actionController;

- (IBAction)addAction:(id)sender;
- (IBAction)removeAction:(id)sender;

@property (assign) IBOutlet NSPopUpButton *puzzleFilterButton;

- (IBAction)switchActionTextView:(id)sender;
- (IBAction)planetTypeFilterChanged:(id)sender;

@end
