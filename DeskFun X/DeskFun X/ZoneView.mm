//
//  ZoneView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "ZoneView.h"

#import "AppDelegate.h"
#import "GameDataParser.hpp"
#import "Zone.hpp"

#include "Palette.h"

static NSCache *cache = nil;

@interface ZoneView ()
{
    Zone *zone;
    id _eventMonitor;
    NSPoint tileHighlight;
}
@property NSImage *image;
@end

@implementation ZoneView

+ (void)initialize
{
    if (self == [ZoneView class]) {
        cache = [[NSCache alloc] init];
        [cache setTotalCostLimit:300];
    }
}

- (void)awakeFromNib
{
    tileHighlight = NSMakePoint(-1, -1);
}

- (void)viewDidMoveToSuperview
{
    NSView *superview = [self superview];
    if(superview)
    {
        _eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSMouseMovedMask|NSTrackingActiveWhenFirstResponder handler:^NSEvent *(NSEvent *e) {
            if([e window] != [self window]) return e;

            NSRect locationInEventWindow = (NSRect){.origin = [e locationInWindow], .size = NSMakeSize(0, 0)};

            NSRect  locationOnScreen = [[e window] convertRectToScreen:locationInEventWindow];
            NSPoint locationInWindow = [[self window] convertRectFromScreen:locationOnScreen].origin;

            NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

            NSRect bounds = [self bounds];
            if(bounds.size.width > bounds.size.height)
                bounds.size.width = bounds.size.height;
            else bounds.size.height = bounds.size.width;

            bounds.origin.x = (self.bounds.size.width - bounds.size.width)/2;
            bounds.origin.y = (self.bounds.size.height - bounds.size.height)/2;

            NSPoint locationOnZone = (NSPoint){
                .x = locationInView.x - NSMinX(bounds),
                .y = locationInView.y - NSMinY(bounds)
            };

            if(!zone)
                return e;

            Sizei zoneSize = zone->getSize();

            NSSize tileSize = (NSSize){
                .width = NSWidth(bounds) / (CGFloat)zoneSize.width,
                .height = NSHeight(bounds) / (CGFloat)zoneSize.height
            };

            tileHighlight = (NSPoint){
                .x = floorf(locationOnZone.x / tileSize.width),
                .y = floorf((NSHeight(bounds)-locationOnZone.y) / tileSize.height),
            };

            [self setNeedsDisplay:YES];
            return e;
        }];
    } else {
        [NSEvent removeMonitor:_eventMonitor];
    }
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];

    NSRect bounds = [self bounds];
    [[NSColor grayColor] setFill];
    NSRectFill(bounds);

    if(!zone) return;


    if(bounds.size.width > bounds.size.height)
        bounds.size.width = bounds.size.height;
    else bounds.size.height = bounds.size.width;

    bounds.origin.x = (self.bounds.size.width - bounds.size.width)/2;
    bounds.origin.y = (self.bounds.size.height - bounds.size.height)/2;

    [[self image] drawInRect:bounds fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];


    Sizei zoneSize = zone->getSize();
    if(tileHighlight.x < 0 || tileHighlight.y < 0 || tileHighlight.x >= zoneSize.width || tileHighlight.y >= zoneSize.height) return;

    NSSize tileSize = (NSSize){
        .width  = NSWidth(bounds)  / (CGFloat)zoneSize.width,
        .height = NSHeight(bounds) / (CGFloat)zoneSize.height
    };

    [[[NSColor redColor] colorWithAlphaComponent:0.4] setFill];
    NSRectFillUsingOperation(NSMakeRect(NSMinX(bounds) + tileSize.width * tileHighlight.x,
                                        NSMaxY(bounds) - tileSize.height * tileHighlight.y - tileSize.height,
                                        tileSize.width, tileSize.height),
                             NSCompositeSourceOver);

    NSString *highlightedTileIndicator = [NSString stringWithFormat:@"Tile: %2dx%2d (%02xx%02x)", (int)tileHighlight.x, (int)tileHighlight.y, (int)tileHighlight.x, (int)tileHighlight.y];

    NSString *tileIDs = @"";
    for(int i=0; i < 3; i++)
    {
        Tile *tile = zone->getTile(tileHighlight.x, tileHighlight.y, i);
        if(tile)
            tileIDs = [tileIDs stringByAppendingFormat:@"%4hx ", tile->index];
    }
    highlightedTileIndicator = [highlightedTileIndicator stringByAppendingFormat:@"\n%@", tileIDs];

    std::vector<Hotspot*> v = zone->getHotspots();
    for(int i=0; i < v.size(); i++)
    {
        Hotspot* st = v[i];

        int x = st->x;
        int y = st->y;

        int type = st->type;
        int arg1 = st->arg1;
        int arg2 = st->arg2;

        if(x != tileHighlight.x || y != tileHighlight.y) continue;

        NSString *string = [NSString stringWithFormat:@"\n%d: %@(%x, %x)", i, [self HotspotTypeName:type], arg1, arg2];
        highlightedTileIndicator = [highlightedTileIndicator stringByAppendingString:string];
    }


    NSShadow *shadow = [[NSShadow alloc] init];
    [shadow setShadowColor:[NSColor blackColor]];
    [shadow setShadowOffset:NSMakeSize(0, -1)];
    [shadow setShadowBlurRadius:0];
    NSFont *font = [NSFont fontWithName:@"Anonymous Pro" size:15.0];
    NSDictionary *attributes = @{ NSShadowAttributeName : shadow,
                                  NSFontAttributeName : font,
                                  NSForegroundColorAttributeName : [NSColor whiteColor],
                                 };

    NSRect textFrame = NSInsetRect(bounds, 7.5, 7.5);;
    [highlightedTileIndicator drawInRect:textFrame withAttributes:attributes];
}

- (NSString*)HotspotTypeName:(uint32_t)type
{
    switch (type)
    {
        case 0x00: return @"TriggerLocation";
        case 0x01: return @"SpawnLocation";
        case 0x02: return @"ForceLocation";
        case 0x03: return @"VehicleTo";
        case 0x04: return @"VehicleBack";
        case 0x05: return @"LocatorThingy";
        case 0x06: return @"CrateItem";
        case 0x07: return @"PuzzleNPC";
        case 0x08: return @"CrateWeapon";
        case 0x09: return @"DoorIn";
        case 0x0A: return @"DoorOut";
        case 0x0B: return @"Unused";
        case 0x0C: return @"Lock";
        case 0x0D: return @"Teleporter";
        case 0x0E: return @"xWingFromD";
        case 0x0F: return @"xWingToD";
        default: return [NSString stringWithFormat:@"%x", type];
    }
}

- (void)setZoneID:(uint16_t)zoneID
{
    AppDelegate *d = (AppDelegate *)[NSApp delegate];
    GameDataParser *data = [d GameDataParser];

    if(zoneID < data->getZoneCount())
    {
        Zone *z = data->getZone(zoneID);
        [self setZone:z];
    }
    else
    {
        [self setImage:nil];
        [self setNeedsDisplay:YES];
    }
}


- (void)setZone:(Zone*)z
{
    zone = z;

    NSImage *image = [self imageForZone:zone];

    [self setImage:image];
    [self setNeedsDisplay:YES];
}

- (NSImage*)imageForZone:(Zone*)z
{
    NSSize tileSize = NSMakeSize(Tile::WIDTH, Tile::HEIGHT);

    Sizei zoneSize = z->getSize();

    NSNumber *zoneID = @(z->index+1);

    NSImage *image = [cache objectForKey:zoneID];
    if(image) return image;

    CGFloat imageHeight = tileSize.height * zoneSize.height;
    image = [[NSImage alloc] initWithSize:NSMakeSize(tileSize.width * zoneSize.width, imageHeight)];
    [image lockFocus];

    for(int x=0; x < zoneSize.width; x++)
    {
        for(int y=0; y < zoneSize.height; y++)
        {
            for(int l=0; l < 3; l++)
            {
                Tile *t = zone->getTile(x, y, l);
                if(t)
                {
                    NSImage *tileImage = [self imageForTile:*t];
                    NSRect tileRect = NSMakeRect(x*tileSize.width, imageHeight - y*tileSize.height - tileSize.height, tileSize.width, tileSize.height);
                    [tileImage drawInRect:tileRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
                }
            }
        }
    }

    std::vector<Hotspot*> v = z->getHotspots();
    for(int i=0; i < v.size(); i++)
    {
        Hotspot* st = v[i];

        int x = st->x;
        int y = st->y;
        
        NSRect tileRect = NSMakeRect(x*tileSize.width, imageHeight - y*tileSize.height - tileSize.height, tileSize.width, tileSize.height);
        [[[NSColor purpleColor] colorWithAlphaComponent:0.6] setFill];
        NSRectFillUsingOperation(tileRect, NSCompositeSourceOver);
    }

    [image unlockFocus];

    [cache setObject:image forKey:zoneID cost:1];

    return image;
}

- (NSImage*)imageForTile:(Tile&)t
{
    AppDelegate *dele = (AppDelegate *)[NSApp delegate];
    GameDataParser* data = [dele GameDataParser];
    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }

    NSSize tileSize = NSMakeSize(Tile::WIDTH, Tile::HEIGHT);
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(tileSize.width, tileSize.height)];
    [image lockFocus];
    for(int x=0; x < Tile::WIDTH; x++)
        for(int y=0; y < Tile::HEIGHT; y++)
        {
            int pixelIndex = y * Tile::WIDTH + x;
            int colorIndex = t.debugGetData()[pixelIndex];
            unsigned char b = palette[colorIndex*4+0];
            unsigned char g = palette[colorIndex*4+1];
            unsigned char r = palette[colorIndex*4+2];
            CGFloat alpha = colorIndex == 0 ? 0.0 : 1.0;

            [[NSColor colorWithDeviceRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:alpha] setFill];
            NSRectFill(NSMakeRect(x, Tile::HEIGHT-y-1, 1, 1));
        }

    [image unlockFocus];
    return image;
}

- (void)clearZone
{
    [self setImage:nil];
    [self setNeedsDisplay:YES];
}

@end
