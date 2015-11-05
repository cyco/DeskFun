//
//  ActionWindowController.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "TileView.h"

#import "GameDataParser.hpp"
#import "Action.hpp"

@interface ActionWindowController : NSWindowController 

- (void)setGameDataParser:(GameDataParser*)data;
- (void)setAction:(Action*)action;

- (IBAction)removeItem:(id)sender;
- (IBAction)addItem:(id)sender;

- (IBAction)saveChanges:(id)sender;
- (IBAction)resetChanges:(id)sender;

@property (assign) IBOutlet NSPopUpButton *opcode;

@property (assign) IBOutlet NSTextField *arg1;
@property (assign) IBOutlet NSTextField *arg2;
@property (assign) IBOutlet NSTextField *arg3;
@property (assign) IBOutlet NSTextField *arg4;
@property (assign) IBOutlet NSTextField *arg5;

@property (assign) IBOutlet NSTextField *additionalDataLengthField;
@property (assign) IBOutlet NSTextView *additionalDataView;

@property (assign) IBOutlet NSTableView *instructions;
@end
