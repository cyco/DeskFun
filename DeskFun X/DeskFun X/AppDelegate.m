//
//  AppDelegate.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "AppDelegate.h"

#import "DAGameWindowController.h"
#import "TileWindowController.h"

#import "CharsController.h"
#import "ItemsController.h"
#import "SoundsController.h"
#import "ZoneWindowController.h"
#import "TileWindowController.h"
#import "PuzzleWindowController.h"
#import "SetupWindowController.h"
#import "SaveGameWindowController.h"

#import "DASpeechBubbleWindow.h"

@interface AppDelegate ()
{
    GameDataParser *data;
}
@property (strong) NSWindowController *tileWindowController;
@property (strong) NSMutableArray *controllers;

@property (strong) NSWindow *bubbbleWindow;

@property (strong) SaveGameWindowController *saveGameController;
@end

@implementation AppDelegate

static NSString *gameName;
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.controllers = [NSMutableArray array];
    NSURL *dataURL;

    gameName = @"yoda";
    dataURL = [[NSBundle mainBundle] URLForResource:gameName withExtension:@"data"];
    data = new GameDataParser([[dataURL path] cStringUsingEncoding:NSUTF8StringEncoding]);

    NSURL *directoryURL = [NSURL fileURLWithPath:@"/something/sfx/"];
    [self setDataDirectoryURL:directoryURL];

    self.gameWindowController = [[DAGameWindowController alloc] initWithURL:nil];
    [self.gameWindowController showWindow:self];
   // [self saveData:nil];

    NSURL *url = [NSURL fileURLWithPath:[@"~/Desktop/savegame.wld" stringByExpandingTildeInPath]];
    //self.saveGameController = [[SaveGameWindowController alloc] initWithWindowNibName:@""];
    //self.saveGameController.fileURL = url;
   // self.saveGameController.data = data;

    // [self.saveGameController showWindow:nil];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{}

- (GameDataParser*)GameDataParser
{
    return data;
}

- (IBAction)openDebugWindow:(id)sender
{
    switch([sender selectedSegment])
    {
        case 0: {       // Tiles
            TileWindowController *controller = [[TileWindowController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        };
        case 1: {       // Zones
            ZoneWindowController *controller = [[ZoneWindowController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        }
        case 2: {       // Items
            ItemsController *controller = [[ItemsController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        }
        case 3: {       // Sounds
            SoundsController *controller = [[SoundsController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        }
        case 4: {       // Chars
            CharsController *controller = [[CharsController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        }
        case 5: {       // Puzzles
            PuzzleWindowController *controller = [[PuzzleWindowController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        };

        case 6: {       // Setup
            SetupWindowController *controller = [[SetupWindowController alloc] init];
            [controller showWindow:self];
            [[self controllers] addObject:controller];
            break;
        };

        default: break;
    }
}

- (IBAction)saveData:(id)sender
{
    size_t size = data->getData(NULL);
    char* bytes = (char*)malloc(size);

    data->getData(bytes);

    NSData *fileContents = [NSData dataWithBytesNoCopy:bytes length:size];
    NSString *filePath = @"something/Yodesk.dta";
    // filePath = @"~/Desktop/yoda.rewritten";

    NSURL *fileURL = [NSURL fileURLWithPath:[filePath stringByExpandingTildeInPath]];
    [fileContents writeToURL:fileURL atomically:YES];
}

- (IBAction)loadOriginal:(id)sender
{
    NSURL *dataURL = [[NSBundle mainBundle] URLForResource:gameName withExtension:@"data"];
    data = new GameDataParser([[dataURL path] cStringUsingEncoding:NSUTF8StringEncoding]);
}

@end
