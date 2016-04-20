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
#include "MapGenerator.hpp"
#include "win_rand.h"

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
    [self testSample:world_things_dead_1 seed:0xDEAD size:WorldSize_SMALL];
    [self testSample:world_things_08FF_3 seed:0x08FF size:WorldSize_LARGE];
    [self testSample:world_things_BB41_3 seed:0xBB41 size:WorldSize_LARGE];
    [self testSample:world_things_60B2_3 seed:0x60B2 size:WorldSize_LARGE];
    [self testSample:world_things_23C6_3 seed:0x23C6 size:WorldSize_LARGE];
    
    //*/
    /* VALID INVALIDS
     [self testSample:world_things_FCA5_3 seed:0xFCA5 size:WorldSize_LARGE];
     [self testSample:world_things_dead_3 seed:0xDEAD size:WorldSize_LARGE];
     //*/
    /* Failing in map generation
     [self testSample:world_things_568C_3 seed:0x568C size:WorldSize_LARGE];
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
            printf("%dx%d\n", i%10, i/10);
            printf("zone_type %d should be %d\n", doc->world_things[i].zone_type, sample[i*4+0]);
            printf("zone_id %d should be %d\n", doc->world_things[i].zone_id, sample[i*4+1]);
            printf("findItemID %d should be %d\n", doc->world_things[i].findItemID, sample[i*4+2]);
            printf("requiredItemID %d should be %d\n", doc->world_things[i].requiredItemID, sample[i*4+3]);
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

- (int)_generateWorld:(uint16_t)seed withSize:(int)size document:(YodaDocument*)doc map:(uint16*)map {
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
    
    
    win_srand(seed);
    
    MapGenerator generator((WorldSize)size);
    generator.generate();
    
    int puzzle_count = generator.puzzleCount;
    memcpy(map, generator.map, sizeof(uint16) * 100);
    memcpy(puzzles, generator.puzzles, sizeof(uint16) * 100);
    
    int puzzles1_count, puzzles2_count;
    if ( (char)puzzle_count % -2 ) {
        puzzles1_count = (puzzle_count + 1) / 2;
        puzzles2_count = (puzzle_count + 1) / 2;
    } else {
        puzzles2_count = puzzle_count / 2;
        puzzles1_count = puzzle_count / 2 + 1;
    }
    
#pragma mark -
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
        return 0;
    }
    
    doc->puzzle_ids_1.clear();
    doc->puzzle_ids_1.resize(puzzles1_count+1, -1);
    
    doc->puzzle_ids_2.clear();
    doc->puzzle_ids_2.resize(puzzles2_count+1, -1);
    
    doc->worldZones.clear();
    doc->worldZones.resize(100, 0);
    doc->puzzle_ids.push_back(goalID);
    doc->puzzle_ids_1[puzzles1_count] = goalID;
    doc->puzzle_ids_2[puzzles2_count] = goalID;
    
    Puzzle *goalPuzzle = doc->puzzles[goalID];
    doc->goal_puzzle_id_again = goalID;
    doc->goal_tile_id_1 = goalPuzzle->item_1;
    doc->goal_tile_id_2 = goalPuzzle->item_2;
    
    // TODO: add goal puzzle to planet puzzle ids
    if(![self placeTransports:doc map:map]) return 0;
    
    if(![self doLoop0:doc puzzle_count:puzzle_count puzzles2_count:puzzles2_count puzzles:(uint16*)puzzles])
        return 0;
    
    Message("After Loop 1\n");
    Message("After Loop 1\n");
    if(![self loop1:doc zoneType:puzzles2_count-1 puzzles:puzzles v199:puzzles1_count - 1 x_8:puzzles1_count]) return 0;
    
    Message("After Loop 2\n");
    if(![self loop2:doc map:map])
        return 0;
    
    Message("After Loop 3\n");
    int result = doc->Unknown_5((int16*)map);
    if(!result) {
        return [self doCleanup:doc];
    }
    
    for(int i=0; i < 100; i++) {
        if(doc->world_things[i].zone_id >= 0) {
            doc->GetTileProvidedByZonesHotspots(doc->world_things[i].zone_id);
        }
    }
    doc->chosen_zone_ids.erase(doc->chosen_zone_ids.begin());
    doc->RemoveEmptyZoneIdsFromWorld();
    doc->WritePlanetValues();
    
    /*
     doc->contols_puzzle_reuse? = -1;
     doc->world_generation_size = doc->puzzle_ids_2.count + doc->puzzle_ids_1.count + 2 * (y_2 + v204);
     doc->world_generation_time = time(0);
     doc->field_7C = 0;
     */
    Message("End Generate New World\n");
    return 1;
}

#pragma mark -
- (int)placeTransports:(YodaDocument*)doc map:(uint16*)map {
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
            
            int zone_id_10 = -1;
            if ( zone_id_8 < 0 ) continue;
            
            doc->world_things[idx].zone_id = zone_id_8;
            doc->world_things[idx].zone_type = ZONETYPE_TravelStart;
            doc->world_things[idx].requiredItemID = doc->wg_item_id;
            Zone* zone = doc->zones[zone_id_8];
            
            for(Hotspot *hotspot : zone->_hotspots) {
                if(hotspot->type == VehicleTo) {
                    zone_id_10 = hotspot->arg1;
                    break;
                }
            }
            
            if ( zone_id_10 < 0) continue;
            
            int foundTravelTarget = false;
            
            int target_x = 0;
            int target_y = 0;
            for(int y=0; y < 10; y++) {
                if(map[target_x + y * 10] == WORLD_ITEM_TRAVEL_END && !doc->worldZones[target_x + y * 10]) {
                    target_y = y;
                    foundTravelTarget = true;
                    break;
                }
            }
            
            if (!foundTravelTarget) {
                target_x = 0;
                target_y = 0;
                for(int x=0; x < 10; x++) {
                    if(map[x + target_y * 10] == WORLD_ITEM_TRAVEL_END && !doc->worldZones[x + target_y * 10]) {
                        target_x = x;
                        foundTravelTarget = true;
                        break;
                    }
                }
                
                if ( !foundTravelTarget ) {
                    target_x = 9;
                    for(int y=0; y < 10; y++) {
                        if(map[target_x+y*10] == 102 && !doc->worldZones[target_x+y*10]){
                            foundTravelTarget = 1;
                            target_y = y;
                            break;
                        }
                    }
                }
            }

            if ( foundTravelTarget ){
                int16 zone_id_9 = zone_id_10;
                if ( !doc->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = target_x + 10 * target_y;
                    Message("transport loop: %dx%d\n", target_x, target_y);
                    Message("transport loop: %dx%d\n", target_x, target_y);
                    doc->worldZones[idx_3] = doc->zones[zone_id_9];
                    doc->world_things[idx_3].zone_id = zone_id_9;;
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
                
                continue;
            }
            
            target_y = 9;
            target_x = 0;
            for(int x=0; x < 10; x++) {
                if(map[x + target_y * 10] == WORLD_ITEM_TRAVEL_END && !doc->worldZones[x + target_y * 10]) {
                    target_x = x;
                    foundTravelTarget = true;
                }
            }
            
            if ( foundTravelTarget )
            {
                int16 zone_id_9 = zone_id_10;
                if ( !doc->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = target_x + 10 * target_y;
                    Message("transport loop: %dx%d\n", target_x, target_y);
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
    return 1;
}

- (int)loop2:(YodaDocument*)doc map:(uint16*)map {
    int v192 = 0;

    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            WORLD_ITEM zone_2 = (WORLD_ITEM)map[idx];
            int did_not_place_zone = 0;
            doc->field_3394 = -1;
            doc->field_3390 = -1;
            doc->wg_item_id = -1;
            doc->wg_last_added_item_id = -1;
            
            WorldThing *worldThing = &doc->world_things[idx];
            if ( zone_2 == WORLD_ITEM_NONE) continue;
            if(zone_2 == WORLD_ITEM_KEPT_FREE) continue;
            if(doc->worldZones[idx] != NULL) continue;
            
            int16 zone_id_1 = [self doPuzzle:doc x:x y:y type:zone_2 nothing_placed:&did_not_place_zone];
            
            if ( zone_id_1 < 0 ) {
                if (v192 == 1 || v192 == 104 || v192 == 300) continue;
                if (!did_not_place_zone ) continue;
                
                int distance = Map::GetDistanceToCenter(x, y);
                zone_id_1 = doc->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                if ( zone_id_1 > 0 ) continue;
                worldThing->zone_type = ZONETYPE_Empty;
                worldThing->zone_id = zone_id_1;
                /*
                 *(_DWORD *)v192 = doc->zones.ptrs[zone_id_2];
                 worldThing[-1].field_30 = zone_id_2;
                 worldThing->field_C = -1;
                 LOWORD(worldThing->zone_type) = -1;
                 worldThing->required_item_id = -1;
                 */
                doc->AddZoneWithIdToWorld(zone_id_1);
            } else {
                worldThing->zone_id = zone_id_1;
                /*
                 *v193 = doc->zones.ptrs[zone_id_1];
                 worldThing->field_C = -1;
                 */
                if ( worldThing->zone_type != ZONETYPE_Town)
                    doc->AddZoneWithIdToWorld(zone_id_1);
            }
        }
    }
    return 1;
}

- (int)loop1:(YodaDocument*)doc zoneType:(int)puzzleMapIdx puzzles:(uint16*)puzzles v199:(int)v199 x_8:(int)x_8 {
    int idx_6 = 0, v195 = 0;

    for(int zone_type = puzzleMapIdx; zone_type > 0; zone_type--) {
        doc->wg_zone_type = -1;
        doc->wg_last_added_item_id = -1;
        doc->wg_item_id_unknown_2 = -1;
        doc->wg_item_id = -1;
        doc->wg_item_id_unknown_3 = -1;
        doc->field_3390 = -1;
        doc->field_3394 = -1;
        doc->field_3398 = -1;
        idx_6 = 0;

        int x =0, y=0;
        Message("zone_type = %d\n", zone_type);
        
        int breakYLoop = 0;
        for(y=0; !breakYLoop && y < 10; y++) {
            int foundSomething = 0;
            for(x=0; x < 10; x++) {
                if(puzzles[x + 10 * y] == zone_type-1) {
                    foundSomething = 1;
                    break;
                }
            }
            
            if (foundSomething) {
                int zone_2 = doc->puzzle_ids_2[zone_type];
                
                int16 item_1 = doc->puzzles[zone_2]->item_1;
                
                int zone_id_5 = -1;
                while ( 1 ) {
                    if ( zone_id_5 >= 0 ) { breakYLoop=1; break;}
                    if ( v199 == x_8 ) {
                        int distance = Map::GetDistanceToCenter(x, y);
                        zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Goal, v199-1, -1, idx_6, -1, distance, 0);
                        if ( zone_id_5 < 0 ) {
                            return [self doCleanup:doc];
                        }

                        doc->wg_zone_type = ZONETYPE_Goal;
                    } else {
                        int rand_2 = win_rand();
                        Message("random = %x\n", rand_2);
                        ZONE_TYPE type = (ZONE_TYPE)((rand_2 & 1) + 15); // was win_rand() & 1
                        
                        // LOWORD(idx_6) = v199 - 1;
                        // item_id = idx_6 = v199-1;
                        // y_2 = 16 - ((unsigned int)((((unsigned __int64)rand_2 >> 32) ^ abs(rand_2) & 1) - ((unsigned __int64)rand_2 >> 32)) < 1);
                        int distance = Map::GetDistanceToCenter(x, y);
                        zone_id_5 = doc->GetZoneIdWithType(type, zone_type-1, -1, item_1, -1, distance, 0);
                        int break_some_more = 0;
                        if ( zone_id_5 >= 0 ) {
                            doc->wg_zone_type = type;
                            break_some_more = 1;
                        }
                        
                        if (!break_some_more && zone_type == ZONETYPE_Use ) {
                            int distance = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Trade, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return [self doCleanup:doc];
                            }

                            doc->wg_zone_type = ZONETYPE_Trade;
                        } else if(!break_some_more) {
                            int distance = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = doc->GetZoneIdWithType(ZONETYPE_Use, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return [self doCleanup:doc];
                            }

                            doc->wg_zone_type = ZONETYPE_Use;
                        }
                    }
                    if ( zone_id_5 < 0 ) {
                        return [self doCleanup:doc];
                    }
                    
                    doc->field_3394 = -1;
                    doc->AddZoneWithIdToWorld(zone_id_5);
                    
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
                    
                    if ( idx_6 > 200) { breakYLoop=1; break;};
                }
            }
        }

        if ( !v195 ) {
            int distance = Map::GetDistanceToCenter(x, y);
            int zone_id = doc->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
            if ( zone_id >= 0 ) {
                int world_idx_2 = x + 10 * y;
                doc->world_things[world_idx_2].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                doc->worldZones[world_idx_2] = doc->zones[zone_id];
                doc->world_things[world_idx_2].zone_id = zone_id;
                doc->AddZoneWithIdToWorld(zone_id);
            }
        }
    }
    return 1;
}

- (int)doCleanup:(YodaDocument*)doc
{
    doc->puzzle_ids_1.clear();
    doc->puzzle_ids_2.clear();
    doc->item_ids.clear();
    
    vector<uint16> *puzzle_array_1;
    switch (doc->planet) {
        case TATOOINE:
            puzzle_array_1 = &doc->tatooine_puzzle_ids;
            break;
        case HOTH:
            puzzle_array_1 = &doc->hoth_puzzle_ids;
            break;
        case ENDOR:
            puzzle_array_1 = &doc->endor_puzzle_ids;
            break;
            
        default:
            break;
    }
    puzzle_array_1->clear();
    
    for(Quest *quest : doc->providedItems)
        delete quest;
    doc->providedItems.clear();
    
    for(Quest *quest : doc->requiredItems)
        delete quest;
    doc->requiredItems.clear();
    
    doc->puzzle_ids.clear();
    doc->chosen_zone_ids.clear();
    Message("-= FAILURE 3 =-\n");
    return 0;
}

- (int)doPuzzle:(YodaDocument*)doc x:(int)x y:(int)y type:(WORLD_ITEM)zone_2 nothing_placed:(int*)did_not_place_zone {
    int distance = Map::GetDistanceToCenter(x, y);
    WorldThing *worldThing = &doc->world_things[x+10*y];
    ZONE_TYPE type = [self zoneTypeForWorldItem:zone_2];
    
    *did_not_place_zone = 0;
    
    if(type == ZONETYPE_Town) {
        int result = doc->GetZoneIdWithType(type, -1, -1, -1, -1, distance, 0);
        if(result >= 0) worldThing->zone_type = ZONETYPE_Town;
        
        *did_not_place_zone = 1;
        return result;
    }
    
    if(ZONETYPE_BlockadeNorth <= type && type <=  ZONETYPE_BlockadeWest) {
        int result = doc->GetZoneIdWithType(type, -1, -1, -1, -1, distance, 0);
        if(result < 0) {
            *did_not_place_zone = 1;
            return -1;
        }
        worldThing->zone_type = type;
        worldThing->requiredItemID = doc->wg_item_id;
        return result;
    }
    
    return -1;
}

- (ZONE_TYPE)zoneTypeForWorldItem:(WORLD_ITEM)item {
    switch(item){
        case WORLD_ITEM_SPACEPORT: return ZONETYPE_Town;
        case WORLD_ITEM_BLOCK_WEST: return ZONETYPE_BlockadeWest;
        case WORLD_ITEM_BLOCK_EAST: return ZONETYPE_BlockadeEast;
        case WORLD_ITEM_BLOCK_NORTH: return ZONETYPE_BlockadeNorth;
        case WORLD_ITEM_BLOCK_SOUTH: return ZONETYPE_BlockadeSouth;
        default: return ZONETYPE_Empty;
    }
}

- (int)doLoop0:(YodaDocument*)doc puzzle_count:(const int)puzzle_count puzzles2_count:(const int)puzzles2_count puzzles:(uint16*)puzzles {
    int y_1 = 0; // edi@216
    signed int zone_id_3 = 0; // ebp@225
    int x_4 = 1; // [sp-8h] [bp-1F4h]@161
    int v204 = 0; // [sp+0h] [bp-1ECh]@0
    int v206 = 0; // [sp+8h] [bp-1E4h]@0
    
    int zone_id_10 = puzzle_count;
    int zone_id_11 = (int)doc->puzzle_ids_1.size()-1;
    if ( puzzle_count > 0 ){
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
            do {
                int foundSomething = 0;
                for(x=0; x < 10; x++) {
                    if(puzzles[x + 10 * y] == zone_id_10 - 1) {
                        foundSomething = 1;
                        break;
                    }
                }
                
                if (foundSomething) {
                    int world_puz_idx = puzzles[x + 10 * y];
                    
                    int16 item_1 = doc->puzzles[zone_2]->item_1;
                    int16 item_2 = doc->puzzles[zone_2]->item_2;
                    
                    int breakAgain=0;
                    zone_id_3 = -1;
                    while (1) {
                        if ( zone_id_3 >= 0 ) {
                            breakAgain = 1;
                            break;
                        }
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
                            int type = ((random ^ 1) & 1) + 15; // was win_rand() & 1
                            int distance = Map::GetDistanceToCenter(x, y);
                            zone_id_3 = doc->GetZoneIdWithType((ZONE_TYPE)type,
                                                               zone_id_11 - 1,
                                                               -1,
                                                               item_1,
                                                               -1,
                                                               distance,
                                                               0+1);
                            if ( zone_id_3 < 0) {
                                if ( zone_id_10 == ZONETYPE_Use ) {
                                    int distance = Map::GetDistanceToCenter(x, y);
                                    zone_id_3 = doc->GetZoneIdWithType(ZONETYPE_Trade, v204, -1, ZONETYPE_Use, -1, distance, 0);
                                    if ( zone_id_3 < 0 ) break;
                                    doc->wg_zone_type = ZONETYPE_Trade;
                                } else {
                                    int distance = Map::GetDistanceToCenter(x, y);
                                    zone_id_3 = doc->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance, 0);
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
                        int world_idx_1 = x + 10 * y;
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
                            breakAgain = 1;
                            break;
                        }
                        
                        Message("v206 = %d\n", x_4);
                        if(v206 > 200) { breakAgain=1; break; };
                    }
                    
                    if(breakAgain) break;
                    
                    return [self doCleanup:doc];
                }
                
            do_the_continue_thing:
                row++;
                ++y_1;
                
                y++;
                x = 0;
            }
            while ( row < 100); // (__int16 *)&v230;
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
    }
    return 1;
}
@end
