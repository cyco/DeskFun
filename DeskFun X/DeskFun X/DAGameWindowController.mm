//
//  DAGameWindowController.m
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAGameWindowController.h"
#import <DeskFunUI/DABezelView.h>

#import "GameDataParser.hpp"
#import "AppDelegate.h"

#import "DASpeechContoller.h"
#import "InputManager.hpp"

#include "Engine.hpp"
#include "World.hpp"

#include "MapScene.hpp"
#include "ZoneScene.hpp"
#include "PauseScene.hpp"

#include "OpenGLRenderer.hpp"
#include "CocoaSoundMixer.hpp"

#include "HostCallbacks.h"

#include "Palette.h"
#define Item(_LABEL_, _IMAGE_) @{@"name":_LABEL_, @"image":_IMAGE_}
@interface DAGameWindowController ()
{
    GameDataParser  *data;
    GameState *state;

    Engine *engine;
}

@property (strong) id eventMonitor;
#pragma mark -
@property (strong) DASpeechContoller *speechController;
#pragma mark -
@property (strong) NSCursor *cursor;
@property (strong) id       cursorInfo;
@end

@implementation DAGameWindowController

- (instancetype)initWithURL:(NSURL*)url
{
    self = [super initWithWindowNibName:[self windowNibName]];
    if(self)
    {
        AppDelegate *d = (AppDelegate*)[NSApp delegate];
        data  = [d GameDataParser];

        engine = new Engine();
        engine->hostCallbacks.host = (__bridge void*)self;
        engine->hostCallbacks.setCursor      = Host::SetCursor;
        engine->hostCallbacks.updateLocation = Host::IndicateLocation;
        engine->hostCallbacks.updateInventory = Host::UpdateInventory;

        engine->data = data;
        engine->mixer = new CococaSoundMixer();
        engine->mixer->mute(SoundMixer::Type::SoundMaster);

        engine->world = new World(*data);
    }
    return self;
}

- (void)_setupTextures
{
    DASceneView *view = [self gameView];
    NSOpenGLContext *gl_ctx = [view openGLContext];
    [gl_ctx makeCurrentContext];

    engine->renderer = self->_renderer = new OpenGLRenderer();
    engine->init();

    [NSOpenGLContext clearCurrentContext];
}

- (void)_setupMenu
{
    NSMenu *menu = [[NSMenu alloc] init];

    NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:@"F"];
    NSMenu *submenu = [[NSMenu alloc] initWithTitle:@"F"];
    [submenu addItemWithTitle:@"New World" action:NULL keyEquivalent:@""];
    [submenu addItemWithTitle:@"Replay Story" action:NULL keyEquivalent:@""];
    [submenu addItemWithTitle:@"Save World" action:NULL keyEquivalent:@""];
    [submenu addItemWithTitle:@"Load World" action:NULL keyEquivalent:@""];
    [submenu addItem:[NSMenuItem separatorItem]];
    [submenu addItemWithTitle:@"Exit" action:NULL keyEquivalent:@""];
    [item setSubmenu:submenu];
    [menu addItem:item];

    item = [[NSMenuItem alloc] initWithTitle:@"Options" action:NULL keyEquivalent:@"O"];
    submenu = [[NSMenu alloc] initWithTitle:@"O"];
    [submenu addItemWithTitle:@"Combat Difficulty…" action:NULL keyEquivalent:@"C"];
    [submenu addItemWithTitle:@"Game Speed…" action:NULL keyEquivalent:@"G"];
    [submenu addItemWithTitle:@"World Control…" action:NULL keyEquivalent:@"W"];
    [submenu addItemWithTitle:@"Statistics" action:NULL keyEquivalent:@"T"];
    [submenu addItem:[NSMenuItem separatorItem]];
    [submenu addItemWithTitle:@"Sound On" action:NULL keyEquivalent:@"E"];
    [submenu addItemWithTitle:@"Music On" action:NULL keyEquivalent:@"M"];
    [submenu addItem:[NSMenuItem separatorItem]];
    [submenu addItemWithTitle:@"Pause" action:NULL keyEquivalent:@"P"];
    [item setSubmenu:submenu];
    [menu addItem:item];

    item = [[NSMenuItem alloc] initWithTitle:@"Window" action:NULL keyEquivalent:@"W"];
    submenu = [[NSMenu alloc] initWithTitle:@"W"];
    [submenu addItemWithTitle:@"Hide Me!" action:NULL keyEquivalent:@""];
    [item setSubmenu:submenu];
    [menu addItem:item];

    item = [[NSMenuItem alloc] initWithTitle:@"Help" action:NULL keyEquivalent:@"H"];
    submenu = [[NSMenu alloc] initWithTitle:@"H"];
    [submenu addItemWithTitle:@"How To Play" action:NULL keyEquivalent:@""];
    [submenu addItemWithTitle:@"Using Help" action:NULL keyEquivalent:@""];
    [submenu addItem:[NSMenuItem separatorItem]];
    [submenu addItemWithTitle:@"About…" action:NULL keyEquivalent:@""];
    [item setSubmenu:submenu];
    [menu addItem:item];

    [[self menuView] setMenu:menu];
}

- (void)dealloc
{}

#pragma mark - XIB
- (NSString*)windowNibName
{
    return @"DAGameWindowController";
}

- (void)windowDidLoad
{
    [super windowDidLoad];

    [self.inventoryTable.enclosingScrollView.contentView setPostsBoundsChangedNotifications:YES];
    [self.inventoryTable.enclosingScrollView.contentView setPostsFrameChangedNotifications:YES];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(inventoryClipViewDidChangeBounds:) name:NSViewBoundsDidChangeNotification object:self.inventoryTable.enclosingScrollView.contentView];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(inventoryClipViewDidChangeBounds:) name:NSViewFrameDidChangeNotification object:self.inventoryTable.enclosingScrollView.contentView];

    [self _setupMenu];
    [self _setupTextures];

    ZoneScene *scene = new ZoneScene();
    unsigned long zoneID = [self findLoadingZoneID];   zoneID = 0x2;
    Zone *zone = data->_zones[zoneID];
    engine->pushScene(scene);
    engine->world->hero.visible = false;
    scene->setZone(zone);
    [_gameView setEngine:engine];
}

- (unsigned int)findLoadingZoneID
{
    // return 0x002; // tatooine spaceport
    // return 0x078; // indy town
    // return 129; // indy room with fire (palette animation)
    //    return 0x5D; // Dagboah travel
    for(int i=0; i < data->_zones.size(); i++)
    {
        Zone *z = data->_zones[i];
        if(z->getType() == Zone::TypeLoad)
            return i;
    }
    return 0xFFFF;
}

#pragma mark - Inventory -
#pragma mark NSTableViewDataSource
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    NSUInteger count = 0;

    if(engine && engine->world)
        count = engine->world->hero.inventory.size();
    return MAX(count, floor(tableView.visibleRect.size.height / tableView.rowHeight));
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSString *identifier = [tableColumn identifier];

    NSString *label = nil;
    NSImage  *image = nil;

    if(engine && engine->world && row  < engine->world->hero.inventory.size())
    {
        Tile *t = engine->world->hero.inventory[row];

        label = [NSString stringWithCString:t->name.c_str() encoding:NSISOLatin1StringEncoding];
        image = [self imageForTile:t];
    }

    if ([identifier isEqualToString:@"MainCell"]) {
        NSTableCellView *cellView = [tableView makeViewWithIdentifier:identifier owner:self];

        NSTextField *labelField = [cellView viewWithTag:1];
        [labelField setStringValue:label ?: @""];

        NSImageView *imageView = [cellView viewWithTag:2];
        [imageView setImage:image ?: nil];

        DABezelView *bezelView = [[cellView subviews] objectAtIndex:0];
        bezelView.backgroundColor = label ? [NSColor colorWithDeviceRed:211/255.0 green:211/255.0 blue:219/255.0 alpha:1.0] : nil;

        return cellView;
    }
    return nil;
}

static NSCache *tileCache = nil;
- (NSImage*)imageForTile:(Tile*)t
{

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        tileCache = [[NSCache alloc] init];
        [tileCache setTotalCostLimit:20];
    });

    if([tileCache objectForKey:@(t->index+1)])
        return [tileCache objectForKey:@(t->index+1)];

    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL pixelsWide:32 pixelsHigh:32 bitsPerSample:8 samplesPerPixel:4 hasAlpha:YES isPlanar:NO colorSpaceName:NSDeviceRGBColorSpace bitmapFormat:0 bytesPerRow:0 bitsPerPixel:0];

    for(int x=0; x < Tile::WIDTH; x++)
        for(int y=0; y < Tile::HEIGHT; y++)
        {
            int pixelIndex = y * Tile::WIDTH + x;
            int colorIndex = t->debugGetData()[pixelIndex];
            unsigned char b = palette[colorIndex*4+0];
            unsigned char g = palette[colorIndex*4+1];
            unsigned char r = palette[colorIndex*4+2];
            CGFloat alpha = colorIndex == 0 ? 0.0 : 1.0;

            [rep setColor:[NSColor colorWithDeviceRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:alpha] atX:x y:y];
        }

    NSImage *image = [[NSImage
                       alloc] init];
    [image setSize:NSMakeSize(32, 32)];
    [image addRepresentation:rep];

    [tileCache setObject:image forKey:@(t->index+1)];
    return image;
}

#pragma mark - NSTableViewDelegate
- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row
{
    return NO;
}

#pragma mark -
- (BOOL)tableView:(NSTableView *)tableView mouseDownWithEvent:(NSEvent *)event
{
    NSPoint locationInWindow = [event locationInWindow];
    NSPoint locationInView   = [tableView convertPoint:locationInWindow fromView:nil];
    __unused NSInteger row = [tableView rowAtPoint:locationInView];


    return NO;
}

- (BOOL)tableView:(NSTableView *)tableView mouseUpWithEvent:(NSEvent *)event
{
    return YES;
}

#pragma mark -
- (void)inventoryClipViewDidChangeBounds:(NSNotification*)notification
{
    [[self scroller] reflectScrolledClipView:notification.object];
}
@end
