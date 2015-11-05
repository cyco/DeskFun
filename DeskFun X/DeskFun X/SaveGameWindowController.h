//
//  SaveGameWindowController.h
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GameDataParser.hpp"
@interface SaveGameWindowController : NSWindowController
@property (copy) NSURL *fileURL;
@property (nonatomic, assign) GameDataParser *data;
@end
