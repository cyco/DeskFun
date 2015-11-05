//
//  TileWindowController.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TileView.h"
@interface TileWindowController : NSWindowController
- (IBAction)changeTile:(id)sender;
@property (assign) IBOutlet NSTextField *iView;
@property (assign) IBOutlet NSTextView *sView;
@property (assign) IBOutlet TileView *tView;
@end
