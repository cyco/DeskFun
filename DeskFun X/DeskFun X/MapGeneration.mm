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

@interface MapGeneration ()
{
    uint16_t map[100];
    YodaDocument *document;
}
@end

@implementation MapGeneration

- (void)windowDidLoad {
    [super windowDidLoad];
    
    [self _loadMapTiles];
    [self _initializeMap];
    
    self.mapView.map = map;
    
    document = new YodaDocument();
    document->puzzles.clear();
    
    [self _loadZones];
    [self _loadPuzzles];
    [self _loadTiles];
    
    
    [self generateWorld:nil];
}

- (void)_loadMapTiles {
    NSMutableArray *tileImages = [NSMutableArray array];
    
    TileStore &store = self.data->_tiles;
    for(int i=817; i < 837; i++)
        [tileImages addObject:[self imageForTile:*store.GetTile(i)]];
    _mapView.tileImages = tileImages;
}

- (void)_loadZones {
    document->zones.clear();
    for(Zone* zone : self.data->_zones) {
        document->zones.push_back(zone);
    }
}

- (void)_loadTiles {
    document->tiles.clear();
    for(Tile* zone : self.data->_tiles) {
        document->tiles.push_back(zone);
    }
}

- (void)_loadPuzzles {
    document->puzzles.clear();
    vector<Puzzle*> puzzles = self.data->_puzzles;
    for(int i=0; i < puzzles.size(); i++) {
        Puzzle *puzzle = puzzles[i];
        if(i == 0xBD || i==0xC5)
            puzzle->type = PUZZLE_TYPE_DISABLED;
        document->puzzles.insert(document->puzzles.end(), puzzle);
    }
}

- (void)_initializeMap {
    for(int i=0; i < 100; i++) {
        map[i] = 0;
    }
    
    for(int i=0; i < _mapView.tileImages.count; i++) {
        map[i] = i;
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

- (void)_generateWorld:(uint16_t)seed withSize:(int)size {
    if(!document) return;
    
    printf("Generate New World (calc)\n");
    
    self.seedField.stringValue = [NSString stringWithFormat:@"0x%04X", seed];
    self.sizeSlider.doubleValue = size;
    
    uint16_t puzzles[100];
    for(int i=0; i < 100; i++) {
        map[i] = 0;
        puzzles[i] = -1;
    }
    
    document->planet = HOTH;
    
    Map *nmap = new Map();
    int puzzle_count = nmap->generate(seed, (WORLD_SIZE)size);
    memcpy(map, nmap->tiles, sizeof(uint16) * 100);
    memcpy(puzzles, nmap->puzzles, sizeof(uint16) * 100);
    
    [_mapView setNeedsDisplay:true];
    
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
    int hotspot_count = 0; // edx@175
    signed int did_find_travel_zone = 0; // ecx@181
    int class1idx = 0; // edi@182
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
    int zone_2 = 0; // [sp+1Ch] [bp-1D0h]@46
    int y_6 = 0; // [sp+20h] [bp-1CCh]@46
    int x_7 = 0; // [sp+24h] [bp-1C8h]@3
    int transport_count = 0; // [sp+2Ch] [bp-1C0h]@3
    int x = 0; // [sp+30h] [bp-1BCh]@53
    int y_7 = 0; // [sp+3Ch] [bp-1B0h]@3
    int v198 = 0;
    
    // view->field_4C = 1;
    document->field_68 = 0;
    /*
     if ( document->game_count < 1 )
     CWordArray::SetAtGrow(&document->hoth_puzzle_ids, document->hoth_puzzle_ids.count, 0xBDu);// add hoth puzzle
     if ( document->game_count < 10 )
     CWordArray::SetAtGrow(&document->hoth_puzzle_ids, document->hoth_puzzle_ids.count, 0xC5u);// add hoth puzzle
     */
    int16 goalID = document->puzzles_can_be_reused;
    if(goalID < 0) {
        goalID = document->GetNewPuzzleId(-1, -1, (ZONE_TYPE)9999, 0);
    }
    
    if(goalID < 0) {
        return;
    }
    
    document->puzzle_ids_1.clear();
    document->puzzle_ids_1.resize(puzzles1_count+1, -1);
    
    document->puzzle_ids_2.clear();
    document->puzzle_ids_2.resize(puzzles2_count+1, -1);
    
    document->puzzle_ids.push_back(goalID);
    document->puzzle_ids_1[puzzles1_count] = goalID;
    document->puzzle_ids_2[puzzles2_count] = goalID;
    
    Puzzle *goalPuzzle = document->puzzles[goalID];
    document->goal_puzzle_id_again = goalID;
    document->goal_tile_id_1 = goalPuzzle->item_1;
    document->goal_tile_id_2 = goalPuzzle->item_2;
    
    document->worldZones.clear();
    document->worldZones.resize(100, 0);
    
    // TODO: add goal puzzle to planet puzzle ids
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + y * 10;
            
            document->field_3390 = -1;
            document->wg_item_id = -1;
            document->wg_last_added_item_id = -1;
            
            
            if(map[idx] != WORLD_ITEM_TRAVEL_START)
                continue;
            
            if(document->worldZones[idx] == NULL)
                continue;
            
            printf("[WARN] Transports are not implementd yet!\n");
            int distance_11 = Map::GetDistanceToCenter(x_8, y_6);
            
            zone_id_10 = -1;
            int16 zone_id_8 = document->GetZoneIdWithType(ZONETYPE_TravelStart, -1, -1, -1, -1, distance_11, 0);
            
            zone_id_11 = zone_id_8;
            if ( zone_id_8 >= 0 )
            {
                document->world_things[x_8 + 10 * y_6].zone_id = zone_id_8;
                document->world_things[x_8 + 10 * y_6].zone_type = ZONETYPE_TravelStart;
                zone_id = zone_id_11;
                v68 = 0;
                // world_thing_plus_4->required_item_id = document->wg_item_id;
                zone_1 = document->zones[zone_id];
                // zone_2 = zone_1;
                hotspot_count = (int)zone_1->_hotspots.size();
                if ( hotspot_count > 0 )
                {
                    int idx = 0;
                    while ( zone_1->_hotspots[idx]->type != VehicleTo )
                    {
                        idx ++;
                        if ( ++v68 >= hotspot_count )
                            goto LABEL_181;
                    }
                    zone_id_10 = zone_1->_hotspots[idx]->arg1;
                }
            LABEL_181:
                did_find_travel_zone = 0;
                if ( zone_id_10 >= 0 )
                {
                    class1idx = 0;
                    int idx = 0;
                    int mapIdx = 0;
                    x_7 = 0;
                    y_7 = 0;
                    while ( map[mapIdx] != 102 || document->worldZones[idx] )
                    {
                        idx += 10;
                        mapIdx += 5;
                        ++class1idx;
                        if ( mapIdx >= 96 )
                            goto LABEL_188;
                    }
                    did_find_travel_zone = 1;
                    y_7 = class1idx;
                LABEL_188:
                    if ( !did_find_travel_zone )
                    {
                        v76 = 0;
                        int worldZoneIdx = 10 * y_7;
                        int worldItemIdx = 5 * y_7;
                        while ( map[worldItemIdx] != 102 || document->worldZones[worldZoneIdx] )
                        {
                            worldZoneIdx++;
                            worldItemIdx++;
                            if ( ++v76 >= 10 )
                                goto LABEL_195;
                        }
                        did_find_travel_zone = 1;
                        x_7 = v76;
                    LABEL_195:
                        if ( !did_find_travel_zone )
                        {
                            v79 = 0;
                            int worldZoneIdx = 9;
                            x_7 = 9;
                            // v81 = &world[5];
                            int mapIdx = 5;
                            
                            while ( map[mapIdx] != 102 || document->worldZones[worldZoneIdx])
                            {
                                worldZoneIdx += 10;
                                mapIdx += 10;
                                // ++t;
                                if ( mapIdx >= 100)
                                    goto LABEL_202;
                            }
                            did_find_travel_zone = 1;
                            y_7 = v79;
                        }
                    }
                LABEL_202:
                    v82 = 0;
                    if ( did_find_travel_zone ){
                        
                        zone_id_9 = zone_id_10;
                        if ( !document->worldContainsZoneId(zone_id_10) )
                        {
                            int idx_3 = x_7 + 10 * y_7;
                            document->worldZones[idx_3] = document->zones[zone_id_10];
                            document->world_things[idx_3].zone_id = zone_id_10;;
                            document->world_things[idx_3].zone_type = ZONETYPE_TravelEnd;
                            // v87 = zone_2;
                            // v88 = (char *)document + 0x34 * idx_3;
                            /*
                             *((_WORD *)v88 + 0x260) = document->wg_item_id;
                             LOWORD(idx_3) = (_WORD)zone_id_11;
                             *(_DWORD *)transport_count = v87;
                             */
                            document->AddZoneWithIdToWorld(idx_3);
                            document->AddZoneWithIdToWorld(zone_id_9);
                        }
                        
                        
                        //            ++world_thing_plus_4;
                        x_2 = x_2 + 1;
                        transport_count += 4;
                        y_6 += 2;
                    }
                    int worldZoneIdx = 90;
                    int worldIdx = 86;
                    y_7 = 9;
                    x_7 = 0;
                    while ( map[worldIdx] != 102 || document->worldZones[worldZoneIdx] )
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
                        if ( !document->worldContainsZoneId(zone_id_10) )
                        {
                            int idx_3 = x_7 + 10 * y_7;
                            document->worldZones[idx_3] = document->zones[zone_id_10];
                            document->world_things[idx_3].zone_id = zone_id_9;
                            document->world_things[idx_3].zone_type = ZONETYPE_TravelEnd;
                            // v87 = zone_2;
                            // v88 = (char *)document + 0x34 * idx_3;
                            /*
                             *((_WORD *)v88 + 0x260) = document->wg_item_id;
                             LOWORD(idx_3) = (_WORD)zone_id_11;
                             *(_DWORD *)transport_count = v87;
                             */
                            document->AddZoneWithIdToWorld(idx_3);
                            document->AddZoneWithIdToWorld(zone_id_9);
                        }
                    }
                    else
                    {
                        document->RemoveQuestProvidingItem(document->wg_item_id);
                        
                        
                        /*             v89 = (_DWORD *)transport_count;
                         *(_WORD *)y_6 = 1;
                         *v89 = 0;
                         world_thing_plus_4->zone_id = -1;
                         world_thing_plus_4->zone_type = -1;
                         world_thing_plus_4->required_item_id = -1;
                         */
                    }
                }
            }
        }
    }
    printf("After Transport Loop\n");
    
    x = puzzle_count; //TODO: check where the 3 comes from
    x_8 = puzzles1_count;
    zone_id_10 = puzzles1_count+1; // puzzle_count
    zone_id_11 = puzzles2_count+1;
    if ( x <= 0 ) goto LABEL_246;
    
    v198 = puzzle_count-1;
    do {
        document->wg_zone_type = -1;
        document->wg_item_id = -1;
        document->wg_item_id_unknown_2 = -1;
        document->wg_item_id_unknown_3 = -1;
        document->wg_last_added_item_id = -1;
        document->field_3390 = -1;
        document->field_3394 = -1;
        document->field_3398 = -1;
        
        y_1 = 0;
        int row = 0;
        zone_2 = document->puzzle_ids_1[zone_id_11];
        
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
            if ( foundSomething )
            {
                int world_puz_idx = puzzles[x + 10 * y];
                zone_id_3 = -1;
                while (1) {
                    if ( zone_id_3 >= 0 ) goto LABEL_242;
                    if ( zone_id_10 == puzzle_count ) {
                        int16 item_1 = document->puzzles[zone_2]->item_1;
                        int16 item_2 = document->puzzles[zone_2]->item_2;
                        int distance = Map::GetDistanceToCenter(x, y);
                        zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Goal, zone_id_11 - 1, x - 1, item_1, item_2, distance, 1);
                        if ( zone_id_3 < 0 ) break;
                        
                        document->wg_zone_type = ZONETYPE_Goal;
                        document->field_3394 = world_puz_idx - 1;
                        // document->field_3398 = (int)&x_2[-1].field_E + 1;
                    } else {
                        static int count = 0;
                        int random = win_rand();
                        printf("random = %x\n", random);
                        int item_id = -1;
                        switch(count) {
                            case 0: item_id = 466; break;
                            case 1: item_id = 455; break;
                        }
                        count++;
                        
                        int type = ((random ^ 1) & 1) + 15; // was win_rand() & 1
                        distance_12 = Map::GetDistanceToCenter(x, y);
                        zone_id_3 = document->GetZoneIdWithType((ZONE_TYPE)type,
                                                                zone_id_11 - 1,
                                                                -1,
                                                                item_id,
                                                                -1,
                                                                distance_12,
                                                                0+1);
                        if ( zone_id_3 < 0) {
                            if ( zone_id_10 == ZONETYPE_Use ) {
                                distance_13 = Map::GetDistanceToCenter(x, y);
                                zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Trade, v204, -1, ZONETYPE_Use, -1, distance_13, 0);
                                if ( zone_id_3 < 0 ) break;
                                document->wg_zone_type = ZONETYPE_Trade;
                            } else {
                                distance_14 = Map::GetDistanceToCenter(x, y);
                                zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance_14, 0);
                                if ( zone_id_3 < 0 ) break;
                                document->wg_zone_type = ZONETYPE_Use;
                            }
                            document->field_3394 = world_puz_idx - 1;
                        } else {
                            document->wg_zone_type = type;
                            document->field_3394 = world_puz_idx - 1;
                        }
                    }
                    document->AddZoneWithIdToWorld(zone_id_3);
                    if ( zone_id_3 < 0 ) break;
                    world_idx_1 = x + 10 * y;
                    v204 = 1;
                    
                    document->world_things[world_idx_1].zone_type = (ZONE_TYPE)document->wg_zone_type;
                    document->world_things[world_idx_1].zone_id = zone_id_3;
                    /*
                     *((_WORD *)v113 + 610) = document->wg_last_added_item_id?;
                     *((_WORD *)v113 + 611) = document->wg_item_id_unknown_2;
                     *((_WORD *)v113 + 608) = document->wg_item_id;
                     *((_WORD *)v113 + 609) = document->wg_item_id_unknown_3;
                     *((_WORD *)v113 + 612) = document->field_3390;
                     *((_WORD *)v113 + 606) = document->field_3394;
                     *((_WORD *)v113 + 607) = document->field_3398;
                     *((_WORD *)v113 + 624) = 1;
                     */
                    document->worldZones[world_idx_1] = document->zones[zone_id_3];
                    
                    printf("y_2 = %d\n", world_puz_idx);
                    if ( world_puz_idx == 1 ) { // esp = 0007F51C, y_2 = esp+14 == 0x7F530
                        int distance = Map::GetDistanceToCenter(x, y);
                        document->AddProvidedQuestWithItemID(document->wg_item_id, distance);
                    }
                    
                    printf("v206 = %d\n", v206);
                    if (v206 > 200 ) goto LABEL_242;
                    if (v198 == 1) goto LABEL_242;
                }
                
                // TODO: clear panet puzzle id array
                // TODO: clear provided quests
                // TODO: clear required quest
                
                document->puzzle_ids_1.clear();
                document->puzzle_ids_2.clear();
                document->item_ids.clear();
                document->providedItems.clear();
                document->requiredItems.clear();
                document->puzzle_ids.clear();
                document->chosen_zone_ids.clear();
                
                printf("-= FAILURE 2 =-");
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
        printf("x_4 = %d\n", x_4);
        if ( !x_4 ) {
            int distance_1 = Map::GetDistanceToCenter(x, y);
            int zone_id_4 = document->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_1, 0);
            if ( zone_id_4 >= 0 )
            {
                int idx_4 = x + 10 * y;
                document->world_things[idx_4].zone_type = (ZONE_TYPE)document->wg_zone_type;
                document->worldZones[idx_4] = document->zones[zone_id_4];
                document->world_things[idx_4].zone_id = zone_id_4;
                document->AddZoneWithIdToWorld(zone_id_4);
            }
        }
        --zone_type;
        printf("v198 = %d\n", v198);
        --v198;
        zone_id_10--;
        zone_id_11--;
    }
    while ( v198 > 0 );
    
LABEL_246:;
    printf("After Loop 1\n");
    printf("After Loop 1\n");
#pragma mark - Second Loop
    v199 = x_8 - 1;
    if ( x_8 - 1 <= 0 )
        goto LABEL_296;
    
    zone_type = 1;
    
    while ( 2 )
    {
        document->wg_zone_type = -1;
        document->wg_last_added_item_id = -1;
        document->wg_item_id_unknown_2 = -1;
        document->wg_item_id = -1;
        document->wg_item_id_unknown_3 = -1;
        document->field_3390 = -1;
        document->field_3394 = -1;
        document->field_3398 = -1;
        idx_6 = 0;
        // v121 = v119[v199];
        v122 = 0;
        // row = &puzzles;
        int x =0, y=0, row = 0;
        printf("zone_type = %d\n", zone_type);

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
            if ( foundSomething )
            {
                zone_id_5 = -1;
                while ( 1 )
                {
                    if ( zone_id_5 >= 0 ) goto LABEL_292;
                    if ( v199 == x_8 ) {
                        distance_15 = Map::GetDistanceToCenter(x, y);
                        /* HIWORD(v136) = HIWORD(v199); */
                        /* LOWORD(v136) = v199 - 1; */
                        v136 = v199-1;
                        zone_id_5 = document->GetZoneIdWithType(ZONETYPE_Goal, v136, -1, idx_6, -1, distance_15, 0);
                        if ( zone_id_5 < 0 )
                            goto LABEL_303;
                        document->wg_zone_type = ZONETYPE_Goal;
                        v138 = v199;
                    }
                    else
                    {
                        int rand_2 = win_rand();
                        printf("random = %x\n", rand_2);
                        int type = (rand_2 & 1) + 15; // was win_rand() & 1

                        static int count2 = 0;
                        int item_id = -1;
                        switch(count2) {
                            case 0: item_id = 1245; break;
                            // case 1: item_id = 1245; break;
                        }
                        count2++;
                        
                        // LOWORD(idx_6) = v199 - 1;
                        // item_id = idx_6 = v199-1;
                        // y_2 = 16 - ((unsigned int)((((unsigned __int64)rand_2 >> 32) ^ abs(rand_2) & 1) - ((unsigned __int64)rand_2 >> 32)) < 1);
                        distance_3 = Map::GetDistanceToCenter(x, y);
                        zone_id_5 = document->GetZoneIdWithType((ZONE_TYPE)type,
                                                                v197,
                                                                -1,
                                                                item_id,
                                                                -1,
                                                                distance_3,
                                                                0 /*v141*/);
                        if ( zone_id_5 >= 0 )
                        {
                            document->wg_zone_type = type;
                            document->field_3394 = y_4 - 1;
                            goto LABEL_290;
                        }
                        if ( zone_type == ZONETYPE_Use )
                        {
                            distance_2 = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = document->GetZoneIdWithType(ZONETYPE_Trade, v195, -1, v197, -1, distance_2, 0);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            document->wg_zone_type = ZONETYPE_Trade;
                        }
                        else
                        {
                            distance_4 = Map::GetDistanceToCenter(x, y);
                            zone_id_5 = document->GetZoneIdWithType(ZONETYPE_Use, v195, -1, v197, -1, distance_4, 0);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            document->wg_zone_type = ZONETYPE_Use;
                        }
                        v138 = v194;
                    }
                    document->field_3394 = v138 - 1;
                LABEL_290:
                    document->AddZoneWithIdToWorld(zone_id_5);
                    if ( zone_id_5 < 0 )
                    {
                    LABEL_303:
                        document->puzzle_ids_1.clear();
                        document->puzzle_ids_2.clear();
                        document->item_ids.clear();
                        
                        planet_7 = document->planet;
                        if ( planet_7 == TATOOINE )
                        {
                            puzzle_ids = &document->tatooine_puzzle_ids;
                        }
                        else if ( planet_7 == HOTH )
                        {
                            puzzle_ids = &document->hoth_puzzle_ids;
                        }
                        else
                        {
                            if ( planet_7 != ENDOR )
                                goto LABEL_311;
                            puzzle_ids = &document->endor_puzzle_ids;
                        }
                        puzzle_ids->clear();
                        
                    LABEL_311:
                        count_1 = (int)document->providedItems.size();
                        if ( count_1 > 0 ) {
                            idx_1 = 0;
                            do {
                                quest_3 = document->providedItems[idx_1];
                                if ( quest_3 )
                                    ; // dealloc quest 3
                                ++idx_1;
                                --count_1;
                            }
                            while ( count_1 );
                        }
                        document->providedItems.clear();
                        
                        count_2 = (int)document->requiredItems.size();
                        if ( count_2 > 0 ) {
                            idx_2 = 0;
                            do
                            {
                                quest_2 = document->requiredItems[idx_2];
                                if ( quest_2 )
                                    ; // deallocate quest_2
                                ++idx_2;
                                --count_2;
                            }
                            while ( count_2 );
                        }
                        document->requiredItems.clear();
                        document->puzzle_ids.clear();
                        document->chosen_zone_ids.clear();
                        
                        printf("-= FAILURE 1 =-\n");
                        return;
                    }
                    ++idx_6;
                    int idx_5 = x + 10 * y;
                    v195 = 1;
                    document->world_things[idx_5].zone_type = (ZONE_TYPE)document->wg_zone_type;
                    document->world_things[idx_5].zone_id = zone_id_5;

                    // v147 = (char *)document + 52 * idx_5;
                    /*
                     *((_WORD *)v147 + 610) = document->wg_last_added_item_id?;
                     *((_WORD *)v147 + 608) = document->wg_item_id;
                     *((_WORD *)v147 + 612) = document->field_3390;
                     *((_WORD *)v147 + 606) = document->field_3394;
                     *((_WORD *)v147 + 624) = 0;
                     */
                    document->worldZones[idx_5] = document->zones[zone_id_5];

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
            zone_id_6 = document->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_5, 0);
            if ( zone_id_6 >= 0 )
            {
                world_idx_2 = x + 10 * y;
                document->world_things[world_idx_2].zone_type = (ZONE_TYPE)document->wg_zone_type;
                document->worldZones[world_idx_2] = document->zones[zone_id_6];
                document->world_things[world_idx_2].zone_id = zone_id_6;
                document->AddZoneWithIdToWorld(zone_id_6);
            }
        }
        --v194;
        if ( --v192 > 0 ) // HIDWORD(v192)
            continue;
        break;
    }
    
    
LABEL_296:
    printf("After Loop 2\n");
#pragma mark - Third Loop
    // worldThing = (WorldThing *)((char *)&document->world_things_1[0] + 8);
    
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            int zone_2 = map[idx];
            
            did_not_place_zone = 0;
            document->field_3394 = -1;
            document->field_3390 = -1;
            document->wg_item_id = -1;
            document->wg_last_added_item_id = -1;
            
            WorldThing *worldThing = &document->world_things[idx];
            
            if ( zone_2 && zone_2 != 305 && !document->worldZones[idx])
            {
                zone_id_1 = -1;
                if ( (unsigned int)((__int16)zone_2 - 201) <= 103 )
                {
                    switch ( (__int16)zone_2 )
                    {
                        case WORLD_ITEM_SPACEPORT:
                            distance_6 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = document->GetZoneIdWithType(ZONETYPE_Town, -1, -1, -1, -1, distance_6, 0);
                            if ( zone_id_1 >= 0 )
                                worldThing->zone_type = ZONETYPE_Town;
                                goto did_not_find_zone_with_required_type;
                        case WORLD_ITEM_BLOCK_WEST:
                            distance_7 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = document->GetZoneIdWithType(ZONETYPE_BlockadeWest, -1, -1, -1, -1, distance_7, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeWest;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_EAST:
                            distance_8 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = document->GetZoneIdWithType(ZONETYPE_BlockadeEast, -1, -1, -1, -1, distance_8, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeEast;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_NORTH:
                            distance_9 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = document->GetZoneIdWithType(ZONETYPE_BlockadeNorth, -1, -1, -1, -1, distance_9, 0);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->zone_type = ZONETYPE_BlockadeNorth;
                            goto LABEL_332;
                        case WORLD_ITEM_BLOCK_SOUTH:
                            distance_16 = Map::GetDistanceToCenter(x, y);
                            zone_id_1 = document->GetZoneIdWithType(ZONETYPE_BlockadeSouth, -1, -1, -1, -1, distance_16, 0);
                            if ( zone_id_1 >= 0 )
                            {
                               worldThing->zone_type = ZONETYPE_BlockadeSouth;
                            LABEL_332:;
                                // worldThing->zone_type = (ZONE_TYPE)document->wg_item_id;
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
                            int zone_id = document->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                            if ( zone_id >= 0 )
                            {
                                zone_id_1 = zone_id;
                                worldThing->zone_type = ZONETYPE_Empty;
                                worldThing->zone_id = zone_id;
                                /*
                                 *(_DWORD *)v192 = document->zones.ptrs[zone_id_2];
                                 worldThing[-1].field_30 = zone_id_2;
                                 worldThing->field_C = -1;
                                 LOWORD(worldThing->zone_type) = -1;
                                 worldThing->required_item_id = -1;
                                 */
                                document->AddZoneWithIdToWorld(zone_id_1);
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    worldThing->zone_id = zone_id_1;
                    /*
                     *v193 = document->zones.ptrs[zone_id_1];
                     worldThing->field_C = -1;
                     */
                    if ( worldThing->zone_type != ZONETYPE_Town)
                        add_zone_to_world:
                        document->AddZoneWithIdToWorld(zone_id_1);
                }
            }
        }
    }
    
    
    printf("After Loop 3\n");
    if ( !document->Unknown_5((int16*)map) ) // &zone_id_11
    {
        document->puzzle_ids_1.clear();
        document->puzzle_ids_2.clear();
        document->item_ids.clear();
        
        planet_4 = document->planet;
        if ( planet_4 == TATOOINE ) {
            puzzle_array_1 = &document->tatooine_puzzle_ids;
        } else if ( planet_4 == HOTH ) {
            puzzle_array_1 = &document->hoth_puzzle_ids;
        } else {
            if ( planet_4 != ENDOR ) {
            clean_up_and_return:
                v180 = (int)document->providedItems.size();
                if ( v180 > 0 ) {
                    v181 = 0;
                    do {
                        v182 = document->providedItems[v181];
                        if ( v182 )
                            ; // deallocate v182
                        
                        ++v181;
                        --v180;
                    }
                    while ( v180 );
                }
                document->providedItems.clear();
                
                count = (int)document->requiredItems.size();
                if ( count > 0 ) {
                    idx = 0;
                    do {
                        v185 = document->requiredItems[idx];
                        if ( v185 ) ; // dealloc v185
                        
                        ++idx;
                        --count;
                    }
                    while ( count );
                }
                document->requiredItems.clear();
                document->puzzle_ids.clear();
                document->chosen_zone_ids.clear();
                printf("-= FAILURE 3 =-\n");
                return;
            }
            puzzle_array_1 = &document->endor_puzzle_ids;
        }
        puzzle_array_1->clear();
        goto clean_up_and_return;
    }
    
    for(int i=0; i < 100; i++) {
         if(document->world_things[i].zone_id >= 0) {
             document->GetTileProvidedByZonesHotspots(document->world_things[i].zone_id);
         }
    }
    
    document->chosen_zone_ids.erase(document->chosen_zone_ids.begin());
    document->RemoveEmptyZoneIdsFromWorld();
    //*/
    
    /*
     switch (document->planet) {
     case TATOOINE: document->WriteTatooineValues(); break;
     case HOTH: document->WriteHothValues(); break;
     case ENDOR: document->WriteEndorValues(); break;
     }
     document->contols_puzzle_reuse? = -1;
     document->world_generation_size = document->puzzle_ids_2.count + document->puzzle_ids_1.count + 2 * (y_2 + v204);
     document->world_generation_time = time(0);
     document->field_7C = 0;
     */
#pragma mark -
    printf("End Generate New World\n");
    printf("\n---===== =====---\n\n");
    printf("document->world_things: (calc)\n");
    for(int i=0; i < 100; i++) {
        WorldThing &thing = document->world_things[i];
        printf("%d: %2d %3d\n", i, thing.zone_type, thing.zone_id);
    }
}
@end
