//
//  TileView.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "TileView.h"
#import "AppDelegate.h"

#include "Tile.hpp"
#import <Cocoa/Cocoa.h>
#include "Palette.h"

#import "TileLayer.h"

@interface TileView ()
@property NSPoint mousePoint;
@property TileLayer *tileLayer;
@end

@implementation TileView
static NSCache *tileCache;
+ (void)initialize
{
    if (self == [TileView class]) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            tileCache = [[NSCache alloc] init];
            [tileCache setCountLimit:100];
        });
    }
}
- (void)awakeFromNib
{
    // self.layer = self.tileLayer = [TileLayer layer];
    // [self setTileID:0x12];
    // [self setWantsLayer:YES];
}

- (NSBitmapImageRep*)imageForTile:(Tile&)t
{

    if([tileCache objectForKey:@(t.index+1)])
        return [tileCache objectForKey:@(t.index+1)];

    AppDelegate *dele = (AppDelegate *)[NSApp delegate];
    GameDataParser* data = [dele GameDataParser];
    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL pixelsWide:32 pixelsHigh:32 bitsPerSample:8 samplesPerPixel:4 hasAlpha:YES isPlanar:NO colorSpaceName:NSDeviceRGBColorSpace bitmapFormat:0 bytesPerRow:0 bitsPerPixel:0];

    for(int x=0; x < Tile::WIDTH; x++)
        for(int y=0; y < Tile::HEIGHT; y++)
        {
            int pixelIndex = y * Tile::WIDTH + x;
            int colorIndex = t.debugGetData()[pixelIndex];
            unsigned char b = palette[colorIndex*4+0];
            unsigned char g = palette[colorIndex*4+1];
            unsigned char r = palette[colorIndex*4+2];
            CGFloat alpha = colorIndex == 0 ? 0.0 : 1.0;

            [rep setColor:[NSColor colorWithDeviceRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:alpha] atX:x y:y];
        }

    [tileCache setObject:rep forKey:@(t.index+1)];
    return rep;
}


- (void)setTileID:(uint16_t)tid
{
    Tile *t = NULL;
    if(tid < GameContext::CurrentContext()->getTileCount()) {
        t = GameContext::CurrentContext()->getTile(tid);
    }
    [self setTile:t];

    /* Write Tiles
    for(int i=0; i < 0xFFFF; i++)
    {
        Tile *t = data.getTile(i);
        if(!t) return;
        NSBitmapImageRep *rep = [self imageForTile:*t];
        [[rep representationUsingType:NSPNGFileType properties:@{}] writeToFile:[[NSString stringWithFormat:@"~/Desktop/tiles/%04d.png", i] stringByExpandingTildeInPath] atomically:YES];
    }
     //*/
}

- (void)setTile:(Tile *)tile
{
    _tile = tile;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [[NSColor whiteColor] setFill];
    NSRectFill([self bounds]);

    if(self.tile)
    {
        NSBitmapImageRep *tileImage = [self imageForTile:*self.tile];
        [tileImage drawInRect:[self bounds] fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:true hints:@{}];
    }
}
#pragma mark -
- (void)mouseDown:(NSEvent *)theEvent
{
}
@end
