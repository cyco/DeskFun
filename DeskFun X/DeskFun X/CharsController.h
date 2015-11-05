//
//  CharsController.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TileView.h"

@interface CharsController : NSWindowController
@property (assign) IBOutlet NSTableView *tableView;

@property (assign) IBOutlet TileView *left,  *left_ex;
@property (assign) IBOutlet TileView *right, *right_ex;
@property (assign) IBOutlet TileView *down, *down_ex;
@property (assign) IBOutlet TileView *up, *up_ex;

@property (assign) IBOutlet NSTextField *auxField;
@property (assign) IBOutlet NSTextView  *specsField;
@property (assign) IBOutlet NSTextField *weaponField;

- (IBAction)step:(id)sender;
@end
