//
//  MapGeneration.m
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#import "MapGeneration.h"
#import "TileStore.hpp"
#import "Palette.h"
#import "AppDelegate.h"
#import "YodaDocument.hpp"
#include "WorldGenerator.hpp"
#include "win_rand.h"

static int logging;

@interface MapGeneration ()
{
    NSLock *generationLock;
    uint16_t _map[100];
    YodaDocument *document;
}
@end

@implementation MapGeneration

- (void)windowDidLoad {
    [super windowDidLoad];
    
    [self _loadMapTiles];
    [self _initializeMap];
    
    self.mapView.map = _map;
    
    generationLock = [[NSLock alloc] init];
    [generationLock lock];
    [self _testSamples];
    [generationLock unlock];
    
    document = new YodaDocument("/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/yodesk.dta");
    
    [self generateWorld:nil];
}

- (void)_loadMapTiles {
    NSMutableArray *tileImages = [NSMutableArray array];
    
    TileStore &store = self.data->_tiles;
    for(int i=817; i < 837; i++)
        [tileImages addObject:[self imageForTile:*store.GetTile(i)]];
    _mapView.tileImages = tileImages;
}

- (void)_initializeMap {
    for(int i=0; i < 100; i++) {
        _map[i] = 0;
    }
    
    for(int i=0; i < _mapView.tileImages.count; i++) {
        _map[i] = i;
    }
}

- (NSBitmapImageRep*)imageForTile:(Tile&)t
{
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
    
    return rep;
}

- (NSString*)windowNibName {
    return @"MapGeneration";
}

#pragma mark - UI
- (IBAction)previousWorld:(id)sender
{
    [self _generateWorld:[self readSeed]-1 withSize:self.sizeSlider.floatValue];
}

- (IBAction)nextWorld:(id)sender {
    [self _generateWorld:[self readSeed]+1 withSize:self.sizeSlider.floatValue];
}

- (IBAction)randomWorld:(id)sender {
    uint16_t seed = random() % 0xFFFF;
    [self _generateWorld:seed withSize:self.sizeSlider.floatValue];
}

- (IBAction)generateWorld:(id)sender {
    [self _generateWorld:[self readSeed] withSize:self.sizeSlider.floatValue];
}

- (uint16_t)readSeed {
    NSString *stringValue = self.seedField.stringValue;
    uint16_t seed = 0;
    if([stringValue rangeOfString:@"0x"].location == 0 || [stringValue rangeOfString:@"0X"].location == 0) {
        seed = strtol([[stringValue substringFromIndex:2] cStringUsingEncoding:NSUTF8StringEncoding], NULL, 0x10);
    } else {
        seed = strtol([stringValue cStringUsingEncoding:NSUTF8StringEncoding], NULL, 10);
    }
    
    return seed;
}

- (void)_testSamples {}

- (void)testSample:(uint32_t*)sample seed:(uint16)seed size:(WorldSize)size {
    YodaDocument *doc = [self makeNewDocument];
    doc->logging = logging;
    
    uint16 map[100] = {0};
    [self _generateWorld:seed withSize:size document:doc map:map];
    
    int success = true;
    /*
    for(int i=0; i < 100; i++) {
        if(!(doc->worldThings[i].zone_type == sample[i*4+0])
           || !(doc->worldThings[i].zone_id == sample[i*4+1])
           || !(doc->worldThings[i].findItemID == sample[i*4+2])
           || !(doc->worldThings[i].requiredItemID == sample[i*4+3])) {
            printf("%dx%d\n", i%10, i/10);
            printf("zone_type %d should be %d\n", doc->worldThings[i].zone_type, sample[i*4+0]);
            printf("zone_id %d should be %d\n", doc->worldThings[i].zone_id, sample[i*4+1]);
            printf("findItemID %d should be %d\n", doc->worldThings[i].findItemID, sample[i*4+2]);
            printf("requiredItemID %d should be %d\n", doc->worldThings[i].requiredItemID, sample[i*4+3]);
            success = false;
            break;
        }
    }
     */
    
    if(success) printf("[OK] %04X %d\n", seed, size);
    else        printf("[  ] %04X %d\n", seed, size);
}

- (void)_dumpCompleteWorld {
    // [self _dumpCompleteWorldWithDoc:document];
}

- (void)_dumpCompleteWorldWithDoc:(YodaDocument*)doc {
    /*
    Message("uint32_t worldThings_%x_%d[] = { ", doc->seed, doc->size);
    for(int i=0; i < 100; i++) {
        WorldThing &thing = doc->worldThings[i];
        Message("0x%x, 0x%x, 0x%x, 0x%x, ", thing.zone_type, thing.zone_id, thing.findItemID, thing.requiredItemID);
    }
    Message("};\n");
*/
}

- (YodaDocument*)makeNewDocument {
    YodaDocument *doc = new YodaDocument("/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/yodesk.dta");
    return doc;
}

- (void)_generateWorld:(uint16_t)seed withSize:(int)size  {
    if(!document) return;
    
    [generationLock lock];
    logging = 0;
    
    [self _generateWorld:seed withSize:size document:document map:_map];
    [self _dumpCompleteWorld];
    self.seedField.stringValue = [NSString stringWithFormat:@"0x%04X", seed];
    self.sizeSlider.doubleValue = size;
    
    [_mapView setNeedsDisplay:true];
    [generationLock unlock];
}

- (int)_generateWorld:(uint16_t)seed withSize:(int)size document:(YodaDocument*)doc map:(uint16*)map {
    doc->logging = logging;
    
    WorldGenerator generator(doc);
    generator.generateWorld(seed, (WorldSize)size, HOTH);
    Message("Generate New World (c++)\n");
    
    uint16 puzzles[100];
    for(int i=0; i < 100; i++) {
        map[i] = 0;
        puzzles[i] = -1;
    }

    return 1;
}

@end
