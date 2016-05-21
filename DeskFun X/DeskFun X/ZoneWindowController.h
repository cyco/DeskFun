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

#pragma mark -
@property (assign) IBOutlet NSButton *roofVisible;
@property (assign) IBOutlet NSButton *objectVisible;
@property (assign) IBOutlet NSButton *floorVisible;
@property (assign) IBOutlet NSButton *roofActive;
@property (assign) IBOutlet NSButton *objectActive;
@property (assign) IBOutlet NSButton *floorActive;
@property (assign) IBOutlet NSSearchField *tileInput;
@property (assign) IBOutlet TileView *tilePickerView;

- (IBAction)toggleLayerVisibility:(id)sender;
- (IBAction)pickEditingLayer:(id)sender;

@end
