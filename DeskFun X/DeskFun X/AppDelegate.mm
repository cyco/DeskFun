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
#import "MapGeneration.h"

#import "DASpeechBubbleWindow.h"

@interface AppDelegate ()
{
    GameDataParser *data;
}
@property (strong) NSWindowController *tileWindowController;
@property (strong) NSMutableArray *controllers;

@property (strong) NSWindow *bubbbleWindow;

@property (strong) SaveGameWindowController *saveGameController;
@property (strong) MapGeneration *generationController;
@end

@implementation AppDelegate

static NSString *gameName;
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.controllers = [NSMutableArray array];
    NSURL *dataURL;

    gameName = @"yoda";
    dataURL = [NSURL fileURLWithPath:@"/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/yodesk.dta"];
    data = new GameDataParser([[dataURL path] cStringUsingEncoding:NSUTF8StringEncoding]);

    NSURL *directoryURL = [NSURL fileURLWithPath:@"/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/sfx/"];
    [self setDataDirectoryURL:directoryURL];

    // self.gameWindowController = [[DAGameWindowController alloc] initWithURL:nil];
    // [self.gameWindowController showWindow:self];
    // [self saveData:nil];

    // NSURL *url = [NSURL fileURLWithPath:[@"~/Desktop/savegame.wld" stringByExpandingTildeInPath]];
    // self.saveGameController = [[SaveGameWindowController alloc] initWithWindowNibName:@""];
    // self.saveGameController.fileURL = url;
    // self.saveGameController.data = data;

    self.generationController = [[MapGeneration alloc] initWithWindowNibName:@""];
    self.generationController.data = data;
    [self.generationController showWindow:nil];
    
    [self writeHeaders];
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
    filePath = @"~/Desktop/yoda.rewritten";

    NSURL *fileURL = [NSURL fileURLWithPath:[filePath stringByExpandingTildeInPath]];
    [fileContents writeToURL:fileURL atomically:YES];
}

- (IBAction)loadOriginal:(id)sender
{
    NSURL *dataURL = [[NSBundle mainBundle] URLForResource:gameName withExtension:@"data"];
    data = new GameDataParser([[dataURL path] cStringUsingEncoding:NSUTF8StringEncoding]);
}
#pragma mark - Write Headers
- (void)writeHeaders {
    [self writeItemTypes];
    [self writeSoundTypes];
    [self writePuzzleNames];
    [self writeTileImages];
}

- (void)writeItemTypes {
    NSMutableArray* usedNames = [NSMutableArray array];
    NSMutableArray *lines = [NSMutableArray array];
    [lines addObject:@"// ITEM TYPES"];
    
    [lines addObject:@"enum{ "];
    for(int i=0; i < data->getItems().size(); i++) {
        Tile* tile = data->getItems()[i];
        NSString *name = [NSString stringWithCString:tile->name.c_str() encoding:NSISOLatin1StringEncoding];
        name = [self makeEnumNameWithString:name alreadyUsed:usedNames];
        [usedNames addObject:name];
        [lines addObject:[NSString stringWithFormat:@"\t%s = 0x%03x,", [name cStringUsingEncoding:NSUTF8StringEncoding], tile->index]];
    }
    [lines addObject:@"} ITEM_TYPE;"];
    [[lines componentsJoinedByString:@"\n"] writeToFile:[@"~/Desktop/Debugging/Items.h" stringByExpandingTildeInPath] atomically:true encoding:NSUTF8StringEncoding error:nil];
}

- (void)writeSoundTypes {
    NSMutableArray* usedNames = [NSMutableArray array];
    NSMutableArray *lines = [NSMutableArray array];
    [lines addObject:@"// SOUND NAMES"];
    [lines addObject:@"enum { "];
    for(int i=0; i < data->_sounds.size(); i++) {
        NSString *name = [NSString stringWithCString:data->_sounds[i]->getFileName().c_str() encoding:NSISOLatin1StringEncoding];
        name = [self makeEnumNameWithString:name alreadyUsed:usedNames];
        [usedNames addObject:name];
        [lines addObject:[NSString stringWithFormat:@"\t%s = 0x%03x,", [name cStringUsingEncoding:NSUTF8StringEncoding], i]];
    }
    [lines addObject:@"} SOUND_NAME;"];
    [[lines componentsJoinedByString:@"\n"] writeToFile:[@"~/Desktop/Debugging/Sounds.h" stringByExpandingTildeInPath] atomically:true encoding:NSUTF8StringEncoding error:nil];
}

- (void)writePuzzleNames {
    NSMutableArray* usedNames = [NSMutableArray array];
    NSMutableArray *lines = [NSMutableArray array];
    [lines addObject:@"// PUZZLE NAMES"];
    [lines addObject:@"enum { "];
    
    for(int i=0; i < data->_puzzles.size(); i++) {
        Puzzle *puzzle = data->_puzzles[i];
        const char* goal_text = puzzle->getText(2).c_str();
        if(strlen(goal_text) == 0 || puzzle->getType() != PuzzleTypeEnd) {
            continue;
        }
        
        NSString *text = [NSString stringWithCString:goal_text encoding:NSISOLatin2StringEncoding];
        NSError* error;
        NSRegularExpression* expr = [[NSRegularExpression alloc] initWithPattern:@"(([A-Z0-9][A-Zs'0-9]* ){0,3}[A-Z]+)" options:0 error:&error];
        
        NSArray *matches = [expr matchesInString:text options:0 range:NSMakeRange(0, text.length)];
        for(NSTextCheckingResult *match in matches) {
            if(match.range.length == 1) continue;
            if(match.range.length == text.length) continue;

            NSString* name = [NSString stringWithFormat:@"GOAL_%@", [text substringWithRange:[match range]]];
            name = [self makeEnumNameWithString:name alreadyUsed:usedNames];
            [usedNames addObject:name];
            [lines addObject:[NSString stringWithFormat:@"\t%s = 0x%03x,", [name cStringUsingEncoding:NSUTF8StringEncoding], i]];
        }
    }
    
    [lines addObject:@"\tGOAL_CAR = 0x0C5,"];
    [lines addObject:@"} PUZZLE_ID;"];
    [[lines componentsJoinedByString:@"\n"] writeToFile:[@"~/Desktop/Debugging/Puzzles.h" stringByExpandingTildeInPath] atomically:true encoding:NSUTF8StringEncoding error:nil];
}

- (void)writeTileImages {
/*
    for(int i=0; i < GameContext::CurrentContext()->getTileCount(); i++){
        Tile *tile = GameContext::CurrentContext()->getTile(i);
    }
 */
}

- (NSString*)makeEnumNameWithString:(NSString*)name alreadyUsed:(NSArray*)used {
    name = [name lastPathComponent];
    name = [name stringByDeletingPathExtension];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@" " withString:@"_"];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@"'" withString:@""];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@"." withString:@""];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@"(" withString:@"_"];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@")" withString:@""];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@"-" withString:@"_"];
    name = [name.uppercaseString stringByReplacingOccurrencesOfString:@"," withString:@""];
    
    if([name rangeOfCharacterFromSet:[NSCharacterSet decimalDigitCharacterSet]].location == 0) {
        name = [@"h" stringByAppendingString:name];
    }
    
    if([name length] == 0) {
        name = @"UNNAMED";
    }
    
    int index = 0;
    NSString *useableName = name;
    while([used containsObject:useableName]) {
        useableName = [name stringByAppendingFormat:@"%d", ++index];
    }
    
    return useableName;
}

@end
