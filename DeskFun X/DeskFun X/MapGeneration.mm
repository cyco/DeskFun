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
#import "Map.hpp"

#import "complete_samples.h"

#define Message(fmt, ...) if(logging) printf(fmt, ##__VA_ARGS__);

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

- (void)_testSamples {

    //* Valid Maps
    logging = 0;
    [self testSample:world_things_aea6_1 seed:0xaea6 size:WorldSize_SMALL];
    [self testSample:world_things_aea6_2 seed:0xaea6 size:WorldSize_MEDIUM];
    [self testSample:world_things_aea6_3 seed:0xaea6 size:WorldSize_LARGE];
    [self testSample:world_things_42b7_1 seed:0x42B7 size:WorldSize_SMALL];
    [self testSample:world_things_42b7_2 seed:0x42B7 size:WorldSize_MEDIUM];
    [self testSample:world_things_42b7_3 seed:0x42B7 size:WorldSize_LARGE];
    [self testSample:world_things_c698_1 seed:0xc698 size:WorldSize_SMALL];
    [self testSample:world_things_1421_1 seed:0x1421 size:WorldSize_SMALL];
    [self testSample:world_things_ebe9_3 seed:0xEBE9 size:WorldSize_LARGE];
    [self testSample:world_things_e43f_3 seed:0xE43F size:WorldSize_LARGE];
    [self testSample:world_things_1e77_3 seed:0x1E77 size:WorldSize_LARGE];
    [self testSample:world_things_70A3_3 seed:0x70A3 size:WorldSize_LARGE];
    //*/
    logging = 1;
    /*
     [self testSample:world_things_568C_3 seed:0x568C size:WorldSize_LARGE];
     [self testSample:world_things_08FF_3 seed:0x08FF size:WorldSize_LARGE];
     [self testSample:world_things_BB41_3 seed:0xBB41 size:WorldSize_LARGE];
     [self testSample:world_things_FCA5_3 seed:0xFCA5 size:WorldSize_LARGE];
     [self testSample:world_things_60B2_3 seed:0x60B2 size:WorldSize_LARGE];
     [self testSample:world_things_23C6_3 seed:0x23C6 size:WorldSize_LARGE];
     //*/
}

- (void)testSample:(uint32_t*)sample seed:(uint16)seed size:(WorldSize)size {
    YodaDocument *doc = [self makeNewDocument];
    doc->logging = logging;
    Map::logging = logging;
    
    uint16 map[100] = {0};
    [self _generateWorld:seed withSize:size document:doc map:map];
    
    int success = true;
    for(int i=0; i < 100; i++) {
        if(!(doc->world_things[i].zone_type == sample[i*4+0])
           || !(doc->world_things[i].zone_id == sample[i*4+1])
           || !(doc->world_things[i].findItemID == sample[i*4+2])
           || !(doc->world_things[i].requiredItemID == sample[i*4+3])) {
            success = false;
            break;
        }
    }
    
    if(success) printf("[OK] %04X %d\n", seed, size);
    else        printf("[  ] %04X %d\n", seed, size);
}

- (void)_dumpCompleteWorld {
    // [self _dumpCompleteWorldWithDoc:document];
}

- (void)_dumpCompleteWorldWithDoc:(YodaDocument*)doc {
    Message("uint32_t world_things_%x_%d[] = { ", doc->seed, doc->size);
    for(int i=0; i < 100; i++) {
        WorldThing &thing = doc->world_things[i];
        Message("0x%x, 0x%x, 0x%x, 0x%x, ", thing.zone_type, thing.zone_id, thing.findItemID, thing.requiredItemID);
    }
    Message("};\n");
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

- (void)_generateWorld:(uint16_t)seed withSize:(int)size document:(YodaDocument*)doc map:(uint16*)map {
    doc->logging = logging;
    Map::logging = logging;
    
    Message("Generate New World (calc)\n");
    
    uint16 puzzles[100];
    for(int i=0; i < 100; i++) {
        map[i] = 0;
        puzzles[i] = -1;
    }
    
    doc->planet = HOTH;
    doc->seed = seed;
    doc->size = (WorldSize)size;
    
    Map *nmap = new Map();
    int puzzle_count = nmap->generate(seed, (WORLD_SIZE)size);
    memcpy(map, nmap->tiles, sizeof(uint16) * 100);
    memcpy(puzzles, nmap->puzzles, sizeof(uint16) * 100);
    
    int puzzles1_count, puzzles2_count;
    if ( (char)puzzle_count % -2 ) {
        puzzles1_count = (puzzle_count + 1) / 2;
        puzzles2_count = (puzzle_count + 1) / 2;
    } else {
        puzzles2_count = puzzle_count / 2;
        puzzles1_count = puzzle_count / 2 + 1;
    }
    
    // TODO: add goal puzzle to planet array
    // // // // // // // // // // // // // // // // // // // // // // //
    // nmap->print();
    // // // // // // // // // // // // // // // // // // // // // // //
#pragma mark -
    int zone_id = 0; // eax@175
    int v68 = 0; // edi@175
    Zone *zone_1; // ecx@175
    signed int did_find_travel_zone = 0; // ecx@181
    int v76 = 0; // edx@189
    int v79 = 0; // edx@196
    int v82 = 0; // edx@202
    __int16 zone_id_9 = 0; // di@210
    int y_1 = 0; // edi@216
    signed int zone_id_3 = 0; // ebp@225
    __int16 distance_12 = 0; // ax@230
    __int16 distance_13 = 0; // ax@233
    __int16 distance_14 = 0; // ax@235
    int world_idx_1 = 0; // ecx@239
    int v122 = 0; // ecx@248
    signed int zone_id_5 = 0; // ebp@276
    __int16 distance_15 = 0; // ST34_2@279
    int v136 = 0; // eax@279
    int v138 = 0; // eax@280
    __int16 distance_3 = 0; // ax@281
    __int16 distance_2 = 0; // ax@284
    __int16 distance_4 = 0; // ax@286
    __int16 distance_5 = 0; // ax@293
    __int16 zone_id_6 = 0; // ax@293
    int world_idx_2 = 0; // edx@294
    signed int did_not_place_zone = 0; // ecx@298
    signed int zone_id_1 = 0; // edx@301
    Planet planet_7; // eax@303
    vector<uint16> *puzzle_ids; // ecx@307
    int count_1 = 0; // edi@311
    int idx_1 = 0; // esi@312
    Quest *quest_3 = 0; // ecx@313
    int count_2 = 0; // edi@316
    int idx_2 = 0; // esi@317
    Quest *quest_2; // ecx@318
    __int16 distance_6 = 0; // ax@322
    __int16 distance_7 = 0; // ax@324
    __int16 distance_8 = 0; // ax@326
    __int16 distance_9 = 0; // ax@328
    __int16 distance_16 = 0; // ax@330
    Planet planet_4; // eax@347
    vector<uint16> *puzzle_array_1; // ecx@351
    int v180 = 0; // edi@355
    int v181 = 0; // esi@356
    Quest *v182; // ecx@357
    int count = 0; // edi@360
    int idx = 0; // esi@361
    Quest *v185; // ecx@362
    int v192 = 0; // [sp-30h] [bp-21Ch]@0
    int v194 = 0; // [sp-24h] [bp-210h]@0
    int v195 = 0; // [sp-20h] [bp-20Ch]@51
    int y_4 = 0; // [sp-1Ch] [bp-208h]@51
    int v197 = 0; // [sp-18h] [bp-204h]@51
    int v199 = 0; // [sp-14h] [bp-200h]@246
    int idx_6 = 0; // [sp-10h] [bp-1FCh]@248
    int zone_type = 0; // [sp-Ch] [bp-1F8h]@51
    int x_4 = 1; // [sp-8h] [bp-1F4h]@161
    int v204 = 0; // [sp+0h] [bp-1ECh]@0
    int v206 = 0; // [sp+8h] [bp-1E4h]@0
    int zone_id_11; // [sp+Ch] [bp-1E0h]@174
    int x_2 = 0; // [sp+10h] [bp-1DCh]@3
    int zone_id_10 = 0; // [sp+14h] [bp-1D8h]@46
    int x_8 = 0; // [sp+18h] [bp-1D4h]@46
    int y_6 = 0; // [sp+20h] [bp-1CCh]@46
    int x_7 = 0; // [sp+24h] [bp-1C8h]@3
    int transport_count = 0; // [sp+2Ch] [bp-1C0h]@3
    int x = 0; // [sp+30h] [bp-1BCh]@53
    int y_7 = 0; // [sp+3Ch] [bp-1B0h]@3
    
    // view->field_4C = 1;
    doc->field_68 = 0;
    /*
     if ( doc->game_count < 1 )
     CWordArray::SetAtGrow(&doc->hoth_puzzle_ids, doc->hoth_puzzle_ids.count, 0xBDu);// add hoth puzzle
     if ( doc->game_count < 10 )
     CWordArray::SetAtGrow(&doc->hoth_puzzle_ids, doc->hoth_puzzle_ids.count, 0xC5u);// add hoth puzzle
     */
    int16 goalID = doc->puzzles_can_be_reused;
    if(goalID < 0) {
        goalID = doc->GetNewPuzzleId(-1, -1, (ZONE_TYPE)9999, 0);
    }
    
    if(goalID < 0) {
        return;
    }
    
    doc->puzzle_ids_1.clear();
    doc->puzzle_ids_1.resize(puzzles1_count+1, -1);
    
    doc->puzzle_ids_2.clear();
    doc->puzzle_ids_2.resize(puzzles2_count+1, -1);
    
    doc->puzzle_ids.push_back(goalID);
    doc->puzzle_ids_1[puzzles1_count] = goalID;
    doc->puzzle_ids_2[puzzles2_count] = goalID;
    
    Puzzle *goalPuzzle = doc->puzzles[goalID];
    doc->goal_puzzle_id_again = goalID;
    doc->goal_tile_id_1 = goalPuzzle->item_1;
    doc->goal_tile_id_2 = goalPuzzle->item_2;
    
    doc->worldZones.clear();
    doc->worldZones.resize(100, 0);
    
    // TODO: add goal puzzle to planet puzzle ids
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + y * 10;
            
            doc->field_3390 = -1;
            doc->wg_item_id = -1;
            doc->wg_last_added_item_id = -1;
            
            if(map[idx] != WORLD_ITEM_TRAVEL_START)
                continue;
            
            int distance = Map::GetDistanceToCenter(x, y);
            int16 zone_id_8 = doc->GetZoneIdWithType(ZONETYPE_TravelStart, -1, -1, -1, -1, distance, 1);
            
            zone_id_10 = -1;
            zone_id_11 = zone_id_8;
            if ( zone_id_8 < 0 ) continue;
            
            doc->world_things[idx].zone_id = zone_id_8;
            doc->world_things[idx].zone_type = ZONETYPE_TravelStart;
            doc->world_things[idx].requiredItemID = doc->wg_item_id;
            zone_1 = doc->zones[zone_id_8];
            
            zone_id = zone_id_11;
            v68 = 0;
            //int zone_2 = zone_1;
            
            for(Hotspot *hotspot : zone_1->_hotspots) {
                if(hotspot->type == VehicleTo) {
                    zone_id_10 = hotspot->arg1;
                    break;
                }
            }
            
            if ( zone_id_10 < 0) continue;
            
            int targetIndex = 0, foundTravelTarget = 0;
            for(targetIndex=0; targetIndex < 100; targetIndex++)
                if(map[targetIndex] == WORLD_ITEM_TRAVEL_END && !doc->worldZones[targetIndex]) {
                    foundTravelTarget = true;
                    break;
                }
         
            if (foundTravelTarget) {
                Message("transport loop 1\n");
                v76 = 0;
                int worldZoneIdx = 10 * y_7;
                int worldItemIdx = 5 * y_7;
                while ( map[worldItemIdx] != 102 || doc->worldZones[worldZoneIdx] )
                {
                    worldZoneIdx++;
                    worldItemIdx++;
                    if ( ++v76 >= 10 )
                        goto LABEL_195;
                }
                foundTravelTarget = 1;
                x_7 = v76;

            LABEL_195:
                if ( !foundTravelTarget )
                {
                    Message("transport loop 2\n");
                    v79 = 0;
                    int worldZoneIdx = 9;
                    x_7 = 9;
                    // v81 = &world[5];
                    int mapIdx = 5;
                    
                    while ( map[mapIdx] != 102 || doc->worldZones[worldZoneIdx])
                    {
                        worldZoneIdx += 10;
                        mapIdx += 10;
                        // ++t;
                        if ( mapIdx >= 100)
                            goto LABEL_202;
                    }
                    foundTravelTarget = 1;
                    y_7 = v79;
                }
            }
        LABEL_202:
            v82 = 0;
            if ( foundTravelTarget ){
                zone_id_9 = zone_id_10;
                if ( !doc->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = x + 10 * y;
                    doc->worldZones[idx_3] = doc->zones[zone_id_10];
                    doc->world_things[idx_3].zone_id = zone_id_10;;
                    doc->world_things[idx_3].zone_type = ZONETYPE_TravelEnd;
                    doc->world_things[idx_3].requiredItemID = doc->wg_item_id;
                    // v87 = zone_2;
                    // v88 = (char *)doc + 0x34 * idx_3;
                    /*
                     LOWORD(idx_3) = (_WORD)zone_id_8;
                     *(_DWORD *)transport_count = v87;
                     */
                    doc->AddZoneWithIdToWorld(zone_id_8);
                    doc->AddZoneWithIdToWorld(zone_id_9);
                }
                
                // ++world_thing_plus_4;
                idx++;
                x_2 = x_2 + 1;
                transport_count += 4;
                y_6 += 2;
                continue;
            }
            
            int worldZoneIdx = 90;
            int worldIdx = 86;
            y_7 = 9;
            x_7 = 0;
            while ( map[worldIdx] != 102 || doc->worldZones[worldZoneIdx] )
            {
                worldZoneIdx++;
                worldIdx++;
                if ( worldZoneIdx >= 96)
                    goto LABEL_209;
            }
            did_find_travel_zone = 1;
            x_7 = v82;
        LABEL_209:
            if ( did_find_travel_zone )
            {
            LABEL_382:
                zone_id_9 = zone_id_10;
                if ( !doc->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = x + 10 * y;
                    doc->worldZones[idx_3] = doc->zones[zone_id_10];
                    doc->world_things[idx_3].zone_id = zone_id_9;
                    doc->world_things[idx_3].zone_type = ZONETYPE_TravelEnd;
                    doc->world_things[idx_3].requiredItemID = doc->wg_item_id;
                    // v87 = zone_2;
                    // v88 = (char *)doc + 0x34 * idx_3;
                    /*
                     LOWORD(idx_3) = (_WORD)zone_id_8;
                     *(_DWORD *)transport_count = v87;
                     */
                    doc->AddZoneWithIdToWorld(zone_id_8);
                    doc->AddZoneWithIdToWorld(zone_id_9);
                }
            }
            else
            {
                doc->RemoveQuestProvidingItem(doc->wg_item_id);
                /*
                 v89 = (_DWORD *)transport_count;
                 *(_WORD *)y_6 = 1;
                 *v89 = 0;
                 world_thing_plus_4->zone_id = -1;
                 world_thing_plus_4->zone_type = -1;
                 world_thing_plus_4->required_item_id = -1;
                 */
            }
        }
    }
    Message("After Transport Loop\n");
    
    x = puzzle_count; //TODO: check where the 3 comes from
    x_8 = puzzles1_count;
    zone_id_10 = puzzle_count; // puzzle_count
    zone_id_11 = (int)doc->puzzle_ids_1.size()-1;
    if ( x <= 0 ) goto LABEL_246;
    
    do {
        doc->wg_zone_type = -1;
        doc->wg_item_id = -1;
        doc->wg_item_id_unknown_2 = -1;
        doc->wg_item_id_unknown_3 = -1;
        doc->wg_last_added_item_id = -1;
        doc->field_3390 = -1;
        doc->field_3394 = -1;
        doc->field_3398 = -1;
        
        y_1 = 0;
        int row = 0;
        int zone_2 = doc->puzzle_ids_1[zone_id_11];
        
        int x = 0, y = 0;
        do
        {
            int foundSomething = 0;
            for(x=0; x < 10; x++) {
                if(puzzles[x + 10 * y] == zone_id_10 - 1) {
                    foundSomething = 1;
                    break;
                }
            }
            
        LABEL_222:
            if (foundSomething) {
                int world_puz_idx = puzzles[x + 10 * y];
                
                int16 item_1 = doc->puzzles[zone_2]->item_1;
                int16 item_2 = doc->puzzles[zone_2]->item_2;

                zone_id_3 = -1;
                while (1) {
                    if ( zone_id_3 >= 0 ) goto LABEL_242;
                    if ( zone_id_10 == puzzle_count ) {
                        int distance = Map::GetDistanceToCenter(x, y);
                        zone_id_3 = doc->GetZoneIdWithType(ZONETYPE_Goal, zone_id_11 - 1, puzzles2_count-1, item_1, item_2, distance, 1);
                        if ( zone_id_3 < 0 ) break;
                        
                        doc->wg_zone_type = ZONETYPE_Goal;
                        doc->field_3394 = world_puz_idx - 1;
                        // doc->field_3398 = (int)&x_2[-1].field_E + 1;
                    } else {
                        int random = win_rand();
                        Message("random = %x\n", random);
                        int item_id = -1;
                    
                        item_id = item_1;
                        
                        int type = ((random ^ 1) & 1) + 15; // was win_rand() & 1
                        distance_12 = Map::GetDistanceToCenter(x, y);
                        zone_id_3 = doc->GetZoneIdWithType((ZONE_TYPE)type,
                                                                zone_id_11 - 1,
                                                                -1,
                                                                item_id,
                                                                -1,
                                                                distance_12,
                                                                0+1);
                        if ( zone_id_3 < 0) {
                            if ( zone_id_10 == ZONETYPE_Use ) {
                                distance_13 = Map::GetDistanceToCenter(x, y);
                                zone_id_3 = doc->GetZoneIdWithType(ZONETYPE_Trade, v204, -1, ZONETYPE_Use, -1, distance_13, 0);
                                if ( zone_id_3 < 0 ) break;
                                doc->wg_zone_type = ZONETYPE_Trade;
                            } else {
                                distance_14 = Map::GetDistanceToCenter(x, y);
                                zone_id_3 = doc->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance_14, 0);
                                if ( zone_id_3 < 0 ) break;
                                doc->wg_zone_type = ZONETYPE_Use;
                            }
                            doc->field_3394 = world_puz_idx - 1;
                        } else {
                            doc->wg_zone_type = type;
                            doc->field_3394 = world_puz_idx - 1;
                        }
                    }
                    doc->AddZoneWithIdToWorld(zone_id_3);
                    if ( zone_id_3 < 0 ) break;
                    world_idx_1 = x + 10 * y;
                    v204 = 1;
                    
                    doc->world_things[world_idx_1].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                    doc->world_things[world_idx_1].zone_id = zone_id_3;
                    doc->world_things[world_idx_1].findItemID = doc->wg_last_added_item_id;
                    doc->world_things[world_idx_1].unknown606 = doc->field_3394;
                    doc->world_things[world_idx_1].requiredItemID = doc->wg_item_id;
                    /*
                     *((_WORD *)v113 + 611) = doc->wg_item_id_unknown_2;
                     *((_WORD *)v113 + 609) = doc->wg_item_id_unknown_3;
                     *((_WORD *)v113 + 612) = doc->field_3390;
                     *((_WORD *)v113 + 607) = doc->field_3398;
                     *((_WORD *)v113 + 624) = 1;
                     */
                    doc->worldZones[world_idx_1] = doc->zones[zone_id_3];
                    
                    Message("y_2 = %d\n", zone_id_11);
                    if ( zone_id_11 == 1) { // esp = 0007F51C, y_2 = esp+14 == 0x7F530
                        int distance = Map::GetDistanceToCenter(x, y);
                        doc->AddProvidedQuestWithItemID(doc->wg_item_id, distance);

                        Message("v206 = %d\n", x_4);
                        goto LABEL_242;
                    }
                    
                    Message("v206 = %d\n", x_4);
                    if(v206 > 200) goto LABEL_242;
                }
                
                // TODO: clear panet puzzle id array
                // TODO: clear provided quests
                // TODO: clear required quest
                
                doc->puzzle_ids_1.clear();
                doc->puzzle_ids_2.clear();
                doc->item_ids.clear();
                doc->providedItems.clear();
                doc->requiredItems.clear();
                doc->puzzle_ids.clear();
                doc->chosen_zone_ids.clear();
                
                Message("-= FAILURE 2 =-");
                return;
            }
            
        do_the_continue_thing:
            row++;
            ++y_1;
            
            y++;
            x = 0;
        }
        while ( row < 100); // (__int16 *)&v230;
    LABEL_242:
        Message("x_4 = %d\n", x_4);
        if ( !x_4 ) {
            int distance_1 = Map::GetDistanceToCenter(x, y);
            int zone_id_4 = doc->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_1, 0);
            if ( zone_id_4 >= 0 )
            {
                int idx_4 = x + 10 * y;
                doc->world_things[idx_4].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                doc->worldZones[idx_4] = doc->zones[zone_id_4];
                doc->world_things[idx_4].zone_id = zone_id_4;
                doc->AddZoneWithIdToWorld(zone_id_4);
            }
        }
        Message("v198 = %d\n", zone_id_11);
        zone_id_10--;
        zone_id_11--;
    }
    while ( zone_id_11 > 0 );
    
LABEL_246:;
    Message("After Loop 1\n");
    Message("After Loop 1\n");
#pragma mark - Second Loop
    v199 = x_8 - 1;
    if ( x_8 - 1 <= 0 )
        goto LABEL_296;
    
    zone_type = puzzles2_count-1;
    
    while ( 2 )
    {
        doc->wg_zone_type = -1;
        doc->wg_last_added_item_id = -1;
        doc->wg_item_id_unknown_2 = -1;
        doc->wg_item_id = -1;
        doc->wg_item_id_unknown_3 = -1;
        doc->field_3390 = -1;
        doc->field_3394 = -1;
        doc->field_3398 = -1;
        idx_6 = 0;
        // v121 = v119[v199];
        v122 = 0;
        // row = &puzzles;
        int x =0, y=0, row = 0;
        Message("zone_type = %d\n", zone_type);

        while (2)
        {
            // v125 = row; = puzzles[row];
            int foundSomething = 0;
            for(x=0; x < 10; x++) {
                if(puzzles[x + 10 * y] == zone_type-1) {
                    foundSomething = 1;
                    break;
                }
            }
        
            ++v122;
        LABEL_273:
            if (foundSomething) {
                int zone_2 = doc->puzzle_ids_2[zone_type];

                int16 item_1 = doc->puzzles[zone_2]->item_1;
                
                zone_id_5 = -1;
                while ( 1 )
                {
                    if ( zone_id_5 >= 0 ) goto LABEL_292;
                    if ( v199 == x_8 ) {
                        distance_15 = Map::GetDistanceToCenter(x, y);
                        /* HIWORD(v136) = HIWORD(v199); */
                        /* LOWORD(v136) = v199 - 1; */
                        v136 = v199-1;
                        zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Goal, v136, -1, idx_6, -1, distance_15, 0);
                        if ( zone_id_5 < 0 )
                            goto LABEL_303;
                        doc->wg_zone_type = ZONETYPE_Goal;
                        v138 = v199;
                    }
                    else
                    {
                        int rand_2 = win_rand();
                        Message("random = %x\n", rand_2);
                        int type = (rand_2 & 1) + 15; // was win_rand() & 1
                        
                        // LOWORD(idx_6) = v199 - 1;
                        // item_id = idx_6 = v199-1;
                        // y_2 = 16 - ((unsigned int)((((unsigned __int64)rand_2 >> 32) ^ abs(rand_2) & 1) - ((unsigned __int64)rand_2 >> 32)) < 1);
                        distance_3 = Map::GetDistanceToCenter(x, y);
                        zone_id_5 = doc->GetZoneIdWithType((ZONE_TYPE)type,
                                                                zone_type-1,
                                                                -1,
                                                                item_1,
                                                                -1,
                                                                distance_3,
                                                                0 /*v141*/);
                        if ( zone_id_5 >= 0 )
                        {
                            doc->wg_zone_type = type;
                            doc->field_3394 = y_4 - 1;
                            goto LABEL_290;
                        }
                        if ( zone_type == ZONETYPE_Use )
                        {
                            distance_2 = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Trade, v195, -1, v197, -1, distance_2, 0);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            doc->wg_zone_type = ZONETYPE_Trade;
                        }
                        else
                        {
                            distance_4 = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Use, v195, -1, v197, -1, distance_4, 0);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            doc->wg_zone_type = ZONETYPE_Use;
                        }
                        v138 = v194;
                    }
                    doc->field_3394 = v138 - 1;
                LABEL_290:
                    doc->AddZoneWithIdToWorld(zone_id_5);
                    if ( zone_id_5 < 0 )
                    {
                    LABEL_303:
                        doc->puzzle_ids_1.clear();
                        doc->puzzle_ids_2.clear();
                        doc->item_ids.clear();
                        
                        planet_7 = doc->planet;
                        if ( planet_7 == TATOOINE )
                        {
                            puzzle_ids = &doc->tatooine_puzzle_ids;
                        }
                        else if ( planet_7 == HOTH )
                        {
                            puzzle_ids = &doc->hoth_puzzle_ids;
                        }
                        else
                        {
                            if ( planet_7 != ENDOR )
                                goto LABEL_311;
                            puzzle_ids = &doc->endor_puzzle_ids;
                        }
                        puzzle_ids->clear();
                        
                    LABEL_311:
                        count_1 = (int)doc->providedItems.size();
                        if ( count_1 > 0 ) {
                            idx_1 = 0;
                            do {
                                quest_3 = doc->providedItems[idx_1];
                                if ( quest_3 )
                                    ; // dealloc quest 3
                                ++idx_1;
                                --count_1;
                            }
                            while ( count_1 );
                        }
                        doc->providedItems.clear();
                        
                        count_2 = (int)doc->requiredItems.size();
                        if ( count_2 > 0 ) {
                            idx_2 = 0;
                            do
                            {
                                quest_2 = doc->requiredItems[idx_2];
                                if ( quest_2 )
                                    ; // deallocate quest_2
                                ++idx_2;
                                --count_2;
                            }
                            while ( count_2 );
                        }
                        doc->requiredItems.clear();
                        doc->puzzle_ids.clear();
                        doc->chosen_zone_ids.clear();
                        
                        Message("-= FAILURE 1 =-\n");
                        return;
                    }
                    ++idx_6;
                    int idx_5 = x + 10 * y;
                    v195 = 1;
                    doc->world_things[idx_5].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                    doc->world_things[idx_5].zone_id = zone_id_5;
                    doc->world_things[idx_5].findItemID = doc->wg_last_added_item_id;
                    doc->world_things[idx_5].unknown606 = doc->field_3394;
                    doc->world_things[idx_5].requiredItemID = doc->wg_item_id;
                    
                    // v147 = (char *)doc + 52 * idx_5;
                    /*
                     *((_WORD *)v147 + 612) = doc->field_3390;
                     *((_WORD *)v147 + 624) = 0;
                     */
                    doc->worldZones[idx_5] = doc->zones[zone_id_5];

                    if ( idx_6 > 200) goto LABEL_292;
                }
            }
            row += 10;
            ++y;
            if (row < 100)
                continue;
            break;
        }
    LABEL_292:
        if ( !v195 )
        {
            distance_5 = Map::GetDistanceToCenter(x, y);
            zone_id_6 = doc->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_5, 0);
            if ( zone_id_6 >= 0 )
            {
                world_idx_2 = x + 10 * y;
                doc->world_things[world_idx_2].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                doc->worldZones[world_idx_2] = doc->zones[zone_id_6];
                doc->world_things[world_idx_2].zone_id = zone_id_6;
                doc->AddZoneWithIdToWorld(zone_id_6);
            }
        }
        if ( --zone_type > 0 ) // HIDWORD(v192)
            continue;
        break;
    }
    
    
LABEL_296:
    Message("After Loop 2\n");
#pragma mark - Third Loop
    // worldThing = (WorldThing *)((char *)&doc->world_things_1[0] + 8);
    
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            int zone_2 = map[idx];
            
            did_not_place_zone = 0;
            doc->field_3394 = -1;
            doc->field_3390 = -1;
            doc->wg_item_id = -1;
            doc->wg_last_added_item_id = -1;
            
            WorldThing *worldThing = &doc->world_things[idx];
            
            if ( zone_2 && zone_2 != 305 && !doc->worldZones[idx])
            {
                zone_id_1 = -1;
                if ( (unsigned int)((__int16)zone_2 - 201) <= 103 )
                {
                    switch ( (__int16)zone_2 )
                    {
                        case WORLD_ITEM_SPACEPORT:
                            distance_6 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_Town, -1, -1, -1, -1, distance_6, 0);
                            if ( zone_id_1 >= 0 )
                                worldThing->zone_type = ZONETYPE_Town;
                                goto did_not_find_zone_with_required_type;
                        case WORLD_ITEM_BLOCK_WEST:
                            distance_7 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_BlockadeWest, -1, -1, -1, -1, distance_7, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeWest;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_EAST:
                            distance_8 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_BlockadeEast, -1, -1, -1, -1, distance_8, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeEast;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_NORTH:
                            distance_9 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_BlockadeNorth, -1, -1, -1, -1, distance_9, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeNorth;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_SOUTH:
                            distance_16 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_BlockadeSouth, -1, -1, -1, -1, distance_16, 0);
                            if ( zone_id_1 >= 0 )
                            {
                               worldThing->zone_type = ZONETYPE_BlockadeSouth;
                            LABEL_332:;
                                worldThing->requiredItemID = doc->wg_item_id;
                                // worldThing->zone_type = (ZONE_TYPE)doc->wg_item_id;
                            }
                        did_not_find_zone_with_required_type:
                            did_not_place_zone = 1;
                            break;
                        default: break;
                    }
                }
                
                if ( zone_id_1 < 0 ) {
                    if ( v192 != 1 && v192 != 104 && v192 != 300 )
                    {
                        if ( did_not_place_zone )
                        {
                            int distance = Map::GetDistanceToCenter(x, y);
                            int zone_id = doc->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                            if ( zone_id >= 0 )
                            {
                                zone_id_1 = zone_id;
                                worldThing->zone_type = ZONETYPE_Empty;
                                worldThing->zone_id = zone_id;
                                /*
                                 *(_DWORD *)v192 = doc->zones.ptrs[zone_id_2];
                                 worldThing[-1].field_30 = zone_id_2;
                                 worldThing->field_C = -1;
                                 LOWORD(worldThing->zone_type) = -1;
                                 worldThing->required_item_id = -1;
                                 */
                                doc->AddZoneWithIdToWorld(zone_id_1);
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    worldThing->zone_id = zone_id_1;
                    /*
                     *v193 = doc->zones.ptrs[zone_id_1];
                     worldThing->field_C = -1;
                     */
                    if ( worldThing->zone_type != ZONETYPE_Town)
                        add_zone_to_world:
                        doc->AddZoneWithIdToWorld(zone_id_1);
                }
            }
        }
    }
    
    
    Message("After Loop 3\n");
    if ( !doc->Unknown_5((int16*)map) ) // &zone_id_11
    {
        doc->puzzle_ids_1.clear();
        doc->puzzle_ids_2.clear();
        doc->item_ids.clear();
        
        planet_4 = doc->planet;
        if ( planet_4 == TATOOINE ) {
            puzzle_array_1 = &doc->tatooine_puzzle_ids;
        } else if ( planet_4 == HOTH ) {
            puzzle_array_1 = &doc->hoth_puzzle_ids;
        } else {
            if ( planet_4 != ENDOR ) {
            clean_up_and_return:
                v180 = (int)doc->providedItems.size();
                if ( v180 > 0 ) {
                    v181 = 0;
                    do {
                        v182 = doc->providedItems[v181];
                        if ( v182 )
                            ; // deallocate v182
                        
                        ++v181;
                        --v180;
                    }
                    while ( v180 );
                }
                doc->providedItems.clear();
                
                count = (int)doc->requiredItems.size();
                if ( count > 0 ) {
                    idx = 0;
                    do {
                        v185 = doc->requiredItems[idx];
                        if ( v185 ) ; // dealloc v185
                        
                        ++idx;
                        --count;
                    }
                    while ( count );
                }
                doc->requiredItems.clear();
                doc->puzzle_ids.clear();
                doc->chosen_zone_ids.clear();
                Message("-= FAILURE 3 =-\n");
                return;
            }
            puzzle_array_1 = &doc->endor_puzzle_ids;
        }
        puzzle_array_1->clear();
        goto clean_up_and_return;
    }
    
    for(int i=0; i < 100; i++) {
         if(doc->world_things[i].zone_id >= 0) {
             doc->GetTileProvidedByZonesHotspots(doc->world_things[i].zone_id);
         }
    }
    
    doc->chosen_zone_ids.erase(doc->chosen_zone_ids.begin());
    doc->RemoveEmptyZoneIdsFromWorld();
    //*/
    
    /*
     switch (doc->planet) {
     case TATOOINE: doc->WriteTatooineValues(); break;
     case HOTH: doc->WriteHothValues(); break;
     case ENDOR: doc->WriteEndorValues(); break;
     }
     doc->contols_puzzle_reuse? = -1;
     doc->world_generation_size = doc->puzzle_ids_2.count + doc->puzzle_ids_1.count + 2 * (y_2 + v204);
     doc->world_generation_time = time(0);
     doc->field_7C = 0;
     */
#pragma mark -
    Message("End Generate New World\n");
    /*
    Message("\n---===== =====---\n\n");
    Message("doc->world_things: (calc)\n");
    for(int i=0; i < 100; i++) {
        WorldThing &thing = doc->world_things[i];
        Message("%02d: %2d %3d %3d %3d\n", i, thing.zone_type, thing.zone_id, thing.findItemID, thing.requiredItemID);
    }
    //*/
}

@end
