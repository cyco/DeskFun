//
//  AppDelegate.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GameDataParser.hpp"

@interface AppDelegate : NSObject <NSApplicationDelegate>
- (GameDataParser*)GameDataParser;

- (IBAction)openDebugWindow:(id)sender;
- (IBAction)saveData:(id)sender;
- (IBAction)loadOriginal:(id)sender;

@property (strong) NSWindowController *gameWindowController;
@property (strong) NSURL *dataDirectoryURL;
@end

