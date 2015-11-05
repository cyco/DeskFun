//
//  DAGameWindowController.h
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "DASceneView.h"
#import <DeskFunUI/DAInteractiveTableView.h>
#import <DeskFunUI/DAMenuBarView.h>
#import <DeskFunUI/DALocationView.h>
#import "OpenGLRenderer.hpp"

@interface DAGameWindowController : NSWindowController <NSTableViewDataSource, NSTableViewDelegate, DAInteravtiveTableViewDelegate>
- (instancetype)initWithURL:(NSURL*)url;

@property (assign) IBOutlet NSTableView *inventoryTable;
@property (assign) IBOutlet NSScroller  *scroller;
@property (assign) IBOutlet DASceneView *gameView;
@property (assign) IBOutlet DAMenuBarView *menuView;
@property (assign) IBOutlet DALocationView *locationView;
@property OpenGLRenderer *renderer;
@end
