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
    void *view; // eax@1
    signed int j; // esi@3
    int spaceport_x_rand; // eax@3
    int spaceport_y_rand; // eax@3
    int *firstWorldThing_zone_ref; // edi@3
    void *v9; // eax@4
    void *v10; // eax@4
    signed int i; // eax@4
    Planet planet_2; // eax@7
    void *puzzle_array_to_clear; // ecx@11
    int k; // esi@15
    int offset; // edi@16
    Quest *quest_1; // ecx@17
    int v17; // esi@20
    int v18; // edi@21
    Quest *quest; // ecx@22
    Planet planet; // eax@26
    Planet planet_1; // eax@33
    void *puzzle_array; // ecx@38
    WORLD_SIZE world_size; // eax@44
    signed int max1; // edi@46
    signed int base1; // ebp@46
    signed int max3; // esi@46
    int v27; // ecx@49
    int rand; // eax@51
    int rand_1; // eax@51
    signed int v30; // esi@52
    signed int v31; // edi@52
    int v32; // eax@53
    int y; // ebp@53
    int v34; // eax@54
    int world_idx; // ecx@55
    __int16 v36; // ax@58
    __int16 v37; // ax@62
    __int16 v38; // ax@67
    __int16 v39; // ax@71
    signed int v40; // edx@76
    __int16 v41; // ax@94
    __int16 v42; // ax@97
    __int16 v43; // ax@105
    __int16 v44; // ax@108
    __int16 v45; // ax@116
    __int16 v46; // ax@119
    bool array1_was_empty; // zf@121
    bool array1_was_less_than_empty; // sf@121
    __int16 v49; // ax@127
    __int16 v50; // ax@130
    int zone_count; // esi@142
    int zone_offset; // ecx@143
    Zone *zone; // eax@144
    Puzzle **puzzle_ptrs; // ecx@161
    int puzzle_count_1; // ecx@161
    Planet planet_6; // eax@161
    Zone *world_zones_1; // esi@168
    signed int count_3; // edx@168
    void *world_thing_plus_4; // esi@170
    int v65; // ST60_4@174
    int zone_id; // eax@175
    int v68; // edi@175
    Zone *zone_1; // ecx@175
    int hotspot_count; // edx@175
    signed int did_find_travel_zone; // ecx@181
    int class1idx; // edi@182
    int v76; // edx@189
    int *world_item; // eax@189
    int v79; // edx@196
    __int16 *v81; // edi@196
    int v82; // edx@202
    Zone *v83; // eax@203
    __int16 *v84; // edi@203
    __int16 zone_id_9; // di@210
    Zone *v87; // edx@211
    char *v88; // eax@211
    void *v89; // ecx@212
    __int16 *puzzle_ids_1; // ecx@216
    int y_1; // edi@216
    __int16 v93; // ax@216
    int v94; // ecx@216
    int x_1; // esi@217
    signed int zone_id_3; // ebp@225
    Puzzle *v100; // eax@225
    int v101; // ebp@228
    __int16 distance_17; // ST54_2@228
    int v103; // eax@228
    int v104 = 1; // ST58_4@228
    int v105; // eax@230
    __int16 distance_12; // ax@230
    int v107; // ST58_4@230
    __int16 distance_13; // ax@233
    int v109; // ST50_4@233
    __int16 distance_14; // ax@235
    int v111; // ST50_4@235
    int world_idx_1; // ecx@239
    char *v113; // eax@239
    __int16 distance; // ax@240
    int v116; // ST40_4@243
    int idx_4; // edx@244
    __int16 *v119; // ecx@248
    int y_3; // edi@248
    __int16 v121; // ax@248
    int v122; // ecx@248
    __int16 *row; // edx@248
    int x_3; // esi@249
    __int16 *v125; // eax@249
    Planet planet_3; // eax@253
    int v128; // esi@261
    int v129; // edi@262
    Quest *v130; // ecx@263
    int v131; // esi@266
    int v132; // edi@267
    Quest *v133; // ecx@268
    signed int zone_id_5; // ebp@276
    __int16 distance_15; // ST34_2@279
    int v136; // eax@279
    int v137; // ST38_4@279
    int v138; // eax@280
    int rand_2; // eax@281
    __int16 distance_3; // ax@281
    int v141; // ST38_4@281
    __int16 distance_2; // ax@284
    int v143; // ST30_4@284
    __int16 distance_4; // ax@286
    int v145; // ST30_4@286
    int idx_5; // ecx@291
    char *v147; // eax@291
    __int16 distance_5; // ax@293
    int v149; // ST28_4@293
    __int16 zone_id_6; // ax@293
    int world_idx_2; // edx@294
    int y_5; // ebp@296
    WorldThing *worldThing; // edi@296
    int x_5; // esi@297
    signed int did_not_place_zone; // ecx@298
    signed int zone_id_1; // edx@301
    Planet planet_7; // eax@303
    void *puzzle_ids; // ecx@307
    int count_1; // edi@311
    int idx_1; // esi@312
    Quest *quest_3; // ecx@313
    int count_2; // edi@316
    int idx_2; // esi@317
    Quest *quest_2; // ecx@318
    __int16 distance_6; // ax@322
    int v166; // ST20_4@322
    __int16 distance_7; // ax@324
    int v168; // ST20_4@324
    __int16 distance_8; // ax@326
    int v170; // ST20_4@326
    __int16 distance_9; // ax@328
    int v172; // ST20_4@328
    __int16 distance_16; // ax@330
    int v174; // ST20_4@330
    __int16 distance_10; // ax@341
    int v176; // ST18_4@341
    signed int zone_id_2; // ecx@341
    Planet planet_4; // eax@347
    void *puzzle_array_1; // ecx@351
    int v180; // edi@355
    int v181; // esi@356
    Quest *v182; // ecx@357
    int count; // edi@360
    int idx; // esi@361
    Quest *v185; // ecx@362
    __int16 *v186; // edi@366
    signed int v187; // esi@366
    signed int v188; // ebp@367
    Planet planet_5; // eax@372
    uint16 zone_id_7; // [sp-54h] [bp-240h]@0
    int v191; // [sp-36h] [bp-222h]@0
    int64_t v192; // [sp-30h] [bp-21Ch]@0
    void *v193; // [sp-28h] [bp-214h]@0
    int v194; // [sp-24h] [bp-210h]@0
    int v195; // [sp-20h] [bp-20Ch]@51
    int y_4; // [sp-1Ch] [bp-208h]@51
    int v197; // [sp-18h] [bp-204h]@51
    int v198; // [sp-14h] [bp-200h]@51
    int v199; // [sp-14h] [bp-200h]@246
    int idx_6; // [sp-10h] [bp-1FCh]@248
    int zone_type; // [sp-Ch] [bp-1F8h]@51
    int x_4; // [sp-8h] [bp-1F4h]@161
    int y_2; // [sp-4h] [bp-1F0h]@161
    int v204; // [sp+0h] [bp-1ECh]@0
    int v205; // [sp+4h] [bp-1E8h]@0
    int v206; // [sp+8h] [bp-1E4h]@0
    int zone_id_11; // [sp+Ch] [bp-1E0h]@174
    int x_2; // [sp+10h] [bp-1DCh]@3
    int zone_id_10; // [sp+14h] [bp-1D8h]@46
    int x_8; // [sp+18h] [bp-1D4h]@46
    int zone_2; // [sp+1Ch] [bp-1D0h]@46
    int y_6; // [sp+20h] [bp-1CCh]@46
    int x_7; // [sp+24h] [bp-1C8h]@3
    __int16 *world_1; // [sp+28h] [bp-1C4h]@3
    int transport_count; // [sp+2Ch] [bp-1C0h]@3
    int x; // [sp+30h] [bp-1BCh]@53
    Zone **world_zones; // [sp+34h] [bp-1B8h]@3
    int y_7; // [sp+3Ch] [bp-1B0h]@3
    int v221; // [sp+48h] [bp-1A4h]@71
    int placed_transports_count; // [sp+4Ch] [bp-1A0h]@3
    int placed_blockades_count; // [sp+50h] [bp-19Ch]@3
    int array_7_count; // [sp+58h] [bp-194h]@33
    int v227; // [sp+124h] [bp-C8h]@4
    __int16 v228; // [sp+12Eh] [bp-BEh]@199
    int v229; // [sp+1C4h] [bp-28h]@274
    int v230; // [sp+1E4h] [bp-8h]@223

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
            /*
             int distance_11 = Map::GetDistanceToCenter(x_8, y_6);

             zone_id_10 = -1;
             int16 zone_id_8 = document->GetZoneIdWithType(ZONETYPE_TravelStart, -1, -1, -1, -1, distance_11, v65);
             
             zone_id_11 = zone_id_8;
             if ( zone_id_8 >= 0 )
             {
             // world_thing_plus_4->zone_id = zone_id_8;
             zone_id = zone_id_11;
             // world_thing_plus_4->zone_type = ZONETYPE_TravelStart;
             v68 = 0;
             // world_thing_plus_4->required_item_id = document->wg_item_id;
             zone_1 = document->zones[zone_id];
             zone_2 = zone_1;
             hotspot_count = (int)zone_1->_hotspots.size();
             if ( hotspot_count > 0 )
             {
             int idx = 0;
             while ( zone_1->_hotspots[idx].type != VehicleTo )
             {
             idx ++;
             if ( ++v68 >= hotspot_count )
             goto LABEL_181;
             }
             zone_id_10 = zone_1->_hotspots[idx].arg1;
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
             v81 = &world[5];
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
             document->world_things[idx_3].zone_id = zone_id_9;
             v87 = zone_2;
             /*
             v88 = (char *)document + 0x34 * idx_3;
             *((_DWORD *)v88 + 0x12E) = 7;
             *((_WORD *)v88 + 0x260) = document->wg_item_id;
             LOWORD(idx_3) = (_WORD)zone_id_11;
             *(_DWORD *)transport_count = v87;
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
            v87 = zone_2;
             v88 = (char *)document + 0x34 * idx_3;
             *((_DWORD *)v88 + 0x12E) = 7;
             *((_WORD *)v88 + 0x260) = document->wg_item_id;
             LOWORD(idx_3) = (_WORD)zone_id_11;
             *(_DWORD *)transport_count = v87;
            document->AddZoneWithIdToWorld(idx_3);
            document->AddZoneWithIdToWorld(zone_id_9);
        }
    }
    else
    {
        document->RemoveQuestProvidingItem(document->wg_item_id);
         v89 = (_DWORD *)transport_count;
         *(_WORD *)y_6 = 1;
         *v89 = 0;
         world_thing_plus_4->zone_id = -1;
         world_thing_plus_4->zone_type = -1;
         world_thing_plus_4->required_item_id = -1;
    }
}
}
*/
        }
    }
    printf("After Transport Loop\n");
    
    zone_id_11 = x = puzzle_count; //TODO: check where the 3 comes from
    x_8 = puzzles1_count;
    zone_id_10 = puzzles1_count+1; // puzzle_count
    zone_id_11 = puzzles2_count+1;
    if ( x <= 0 )
        goto LABEL_246;

    /*
    printf("World:\n");
    for(int y=0; y < 10; y++){
        for(int x=0; x < 10; x++){
            printf("%3d ", puzzles[x+y*10]);
        }
        printf("\n");
    }
    //*/
    do
    {
        document->wg_zone_type = -1;
        document->wg_item_id = -1;
        document->wg_item_id_unknown_2 = -1;
        document->wg_item_id_unknown_3 = -1;
        document->wg_last_added_item_id = -1;
        document->field_3390 = -1;
        document->field_3394 = -1;
        document->field_3398 = -1;

        y_1 = 0;
        v94 = 0;
        int row = 0;
        zone_2 = document->puzzle_ids_1[zone_id_11];
        x_2 = 0;
        
        int x = 0, y = 0;
        do
        {
            int foundSomething = 0;
            x_1 = 0;
            x = 0;
            while ( puzzles[x+10*y] - zone_id_10 != -1 ) // use puzzles here instead?
            {
                x++;
                if ( ++x >= 10 )
                    goto LABEL_222;
            }
            foundSomething = 1;
            
        LABEL_222:
            if ( foundSomething )
            {
                zone_id_3 = -1;
                y_6 = 0;
                transport_count = document->puzzles[zone_2]->item_2;
                while (1) {
                    if ( zone_id_3 >= 0 )
                        goto LABEL_242;
                    
                    if ( zone_id_10 == puzzle_count ) {
                        zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Goal, zone_id_11 - 1, x - 1, document->puzzles[zone_2]->item_1, document->puzzles[zone_2]->item_2, Map::GetDistanceToCenter(x, y), v104);
                        if ( zone_id_3 < 0 )
                            break;
                        
                        document->wg_zone_type = ZONETYPE_Goal;
                        // document->field_3394 = v205 - 1;
                        // document->field_3398 = (int)&x_2[-1].field_E + 1;
                    }
                    else
                    {
                        v105 = win_rand();
                        zone_2 = (v105 & 1) + 15;
                        x_2 = zone_id_11 - 1;
                        distance_12 = Map::GetDistanceToCenter(x_1, y_1);
                        zone_id_3 = document->GetZoneIdWithType((ZONE_TYPE)zone_id_10, v206, -1, (int)zone_2, -1, distance_12, v107);
                        if ( zone_id_3 < 0 ) {
                            if ( zone_id_10 == ZONETYPE_Use ) {
                                distance_13 = Map::GetDistanceToCenter(x_1, y_1);
                                zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Trade, v204, -1, ZONETYPE_Use, -1, distance_13, v109);
                                if ( zone_id_3 < 0 )
                                    break;
                                document->wg_zone_type = ZONETYPE_Trade;
                            } else {
                                distance_14 = Map::GetDistanceToCenter(x_1, y_1);
                                zone_id_3 = document->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance_14, v111);
                                if ( zone_id_3 < 0 ) break;
                                document->wg_zone_type = ZONETYPE_Use;
                            }
                            document->field_3394 = y_2 - 1;
                        } else {
                            document->wg_zone_type = zone_id_10;
                            document->field_3394 = v205 - 1;
                        }
                    }
                    document->AddZoneWithIdToWorld(zone_id_3);
                    if ( zone_id_3 < 0 ) break;
                    world_idx_1 = x_1 + 10 * y_1;
                    v204 = 1;
                    /*
                    document->world_things_1[world_idx_1].zone_type = document->wg_zone_type;
                    v113 = (char *)document + 0x34 * world_idx_1;
                    *((_WORD *)v113 + 610) = document->wg_last_added_item_id?;
                    *((_WORD *)v113 + 611) = document->wg_item_id_unknown_2;
                    *((_WORD *)v113 + 608) = document->wg_item_id;
                    *((_WORD *)v113 + 609) = document->wg_item_id_unknown_3;
                    *((_WORD *)v113 + 612) = document->field_3390;
                    *((_WORD *)v113 + 606) = document->field_3394;
                    *((_WORD *)v113 + 607) = document->field_3398;
                    *((_WORD *)v113 + 624) = 1;
                    document->world_zones[world_idx_1] = document->zones.ptrs[zone_id_3];
                    *((_WORD *)v113 + 602) = zone_id_3;
                     */
                    if ( y_2 == 1 ) {
                        distance = Map::GetDistanceToCenter(x_1, y_1);
                        document->AddProvidedQuestWithItemID(document->wg_item_id, distance);
                    }
                    
                    if ( ++v206 > 200 )
                        goto LABEL_242;
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
                
                return;
            }
            row++;
            ++y_1;
            
            y++;
            x = 0;
        }
        while ( row < 100); // (__int16 *)&v230;
    LABEL_242:
        if ( !x_4 ) {
            int distance_1 = Map::GetDistanceToCenter((int)x_2, y_2);
            int zone_id_4 = document->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_1, v116);
            if ( zone_id_4 >= 0 )
            {
                idx_4 = x_1 + 10 * y_1;
                document->world_things[idx_4].zone_type = (ZONE_TYPE)document->wg_zone_type;
                document->worldZones[idx_4] = document->zones[zone_id_4];
                document->world_things[idx_4].zone_id = zone_id_4;
                document->AddZoneWithIdToWorld(zone_id_4);
            }
        }
        --zone_type;
        --x_4;
        --v198;
    }
    while ( v198 > 0 );

    printf("After Loop 2\n");
LABEL_246:;
    /*
        v199 = x_8 - 1;
    if ( x_8 - 1 <= 0 )
        goto LABEL_296;
    HIWORD(v197) = 0;
    while ( 2 )
    {
        document->wg_zone_type = -1;
        document->wg_last_added_item_id? = -1;
        document->wg_item_id_unknown_2 = -1;
        document->wg_item_id = -1;
        document->wg_item_id_unknown_3 = -1;
        v119 = document->puzzle_ids_2.ptrs;
        document->field_3390 = -1;
        document->field_3394 = -1;
        y_3 = 0;
        document->field_3398 = -1;
        idx_6 = 0;
        v121 = v119[v199];
        v122 = 0;
        row = &world[80];
        y_2 = (uint16)v121;
        while ( 2 )
        {
            x_3 = 0;
            v125 = row;
            while ( *v125 - zone_type != -1 )
            {
                ++v125;
                if ( ++x_3 >= 10 )
                    goto LABEL_273;
            }
            ++v122;
        LABEL_273:
            if ( v122 )
            {
                zone_id_5 = -1;
                v204 = 0;
                LOWORD(x_4) = document->puzzles.ptrs[y_2]->item_1;
                while ( 1 )
                {
                    if ( zone_id_5 >= 0 )
                        goto LABEL_292;
                    if ( v199 == x_8 )
                    {
                        distance_15 = Map::GetDistanceToCenter(x_3, y_3);
                        HIWORD(v136) = HIWORD(v199);
                        LOWORD(v136) = v199 - 1;
                        zone_id_5 = YodaDocument::GetZoneIdWithType(document, Goal, v136, -1, idx_6, -1, distance_15, v137);
                        if ( zone_id_5 < 0 )
                            goto LABEL_303;
                        document->wg_zone_type = Goal;
                        v138 = v199;
                    }
                    else
                    {
                        rand_2 = win_rand();
                        LOWORD(idx_6) = v199 - 1;
                        y_2 = 16
                        - ((unsigned int)((((unsigned __int64)rand_2 >> 32) ^ abs(rand_2) & 1) - ((unsigned __int64)rand_2 >> 32)) < 1);
                        distance_3 = Map::GetDistanceToCenter(x_3, y_3);
                        zone_id_5 = YodaDocument::GetZoneIdWithType(
                                                                    document,
                                                                    (ZONE_TYPE)zone_type,
                                                                    v197,
                                                                    -1,
                                                                    idx_6,
                                                                    -1,
                                                                    distance_3,
                                                                    v141);
                        if ( zone_id_5 >= 0 )
                        {
                            document->wg_zone_type = zone_type;
                            document->field_3394 = y_4 - 1;
                            goto LABEL_290;
                        }
                        if ( zone_type == Use )
                        {
                            distance_2 = Map::GetDistanceToCenter(x_3, y_3);
                            zone_id_5 = YodaDocument::GetZoneIdWithType(document, Trade, v195, -1, v197, -1, distance_2, v143);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            document->wg_zone_type = 15;
                        }
                        else
                        {
                            distance_4 = Map::GetDistanceToCenter(x_3, y_3);
                            zone_id_5 = YodaDocument::GetZoneIdWithType(document, Use, v195, -1, v197, -1, distance_4, v145);
                            if ( zone_id_5 < 0 )
                                goto LABEL_303;
                            document->wg_zone_type = 16;
                        }
                        v138 = v194;
                    }
                    document->field_3394 = v138 - 1;
                LABEL_290:
                    YodaDocument::AddZoneWithIdToWorld(document, zone_id_5);
                    if ( zone_id_5 < 0 )
                    {
                    LABEL_303:
                        CWordArray::SetSize(&document->puzzle_ids_1, 0, -1);
                        CWordArray::SetSize(&document->puzzle_ids_2, 0, -1);
                        CWordArray::SetSize(&document->item_ids, 0, -1);
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
                        CWordArray::SetSize(puzzle_ids, 0, -1);
                    LABEL_311:
                        count_1 = document->provided_quests.count;
                        if ( count_1 > 0 )
                        {
                            idx_1 = 0;
                            do
                            {
                                quest_3 = document->provided_quests.ptrs[idx_1];
                                if ( quest_3 )
                                    ((void (__stdcall *)(signed int))quest_3->vtable->Deallocate)(1);
                                ++idx_1;
                                --count_1;
                            }
                            while ( count_1 );
                        }
                        CObArray::SetSize((Array *)&document->provided_quests, 0, -1);
                        count_2 = document->required_quests.count;
                        if ( count_2 > 0 )
                        {
                            idx_2 = 0;
                            do
                            {
                                quest_2 = document->required_quests.ptrs[idx_2];
                                if ( quest_2 )
                                    ((void (__stdcall *)(signed int))quest_2->vtable->Deallocate)(1);
                                ++idx_2;
                                --count_2;
                            }
                            while ( count_2 );
                        }
                        CObArray::SetSize((Array *)&document->required_quests, 0, -1);
                        CWordArray::SetSize(&document->puzzle_ids, 0, -1);
                        CWordArray::SetSize(&document->world_zone_ids?, 0, -1);
                        return 0;
                    }
                    ++idx_6;
                    idx_5 = x_3 + 10 * y_3;
                    v195 = 1;
                    document->world_things_1[idx_5].zone_type = document->wg_zone_type;
                    v147 = (char *)document + 52 * idx_5;
                    *((_WORD *)v147 + 610) = document->wg_last_added_item_id?;
                    *((_WORD *)v147 + 608) = document->wg_item_id;
                    *((_WORD *)v147 + 612) = document->field_3390;
                    *((_WORD *)v147 + 606) = document->field_3394;
                    *((_WORD *)v147 + 624) = 0;
                    document->world_zones[idx_5] = document->zones.ptrs[zone_id_5];
                    *((_WORD *)v147 + 602) = zone_id_5;
                    if ( idx_6 > 200 )
                        goto LABEL_292;
                }
            }
            row += 10;
            ++y_3;
            if ( row < (__int16 *)&v229 )
                continue;
            break;
        }
    LABEL_292:
        if ( !v195 )
        {
            distance_5 = Map::GetDistanceToCenter(x_4, y_4);
            zone_id_6 = YodaDocument::GetZoneIdWithType(document, EMPTY, -1, -1, -1, -1, distance_5, v149);
            if ( zone_id_6 >= 0 )
            {
                world_idx_2 = x_3 + 10 * y_3;
                document->world_things_1[world_idx_2].zone_type = document->wg_zone_type;
                document->world_zones[world_idx_2] = document->zones.ptrs[zone_id_6];
                document->world_things_1[world_idx_2].zone_id = zone_id_6;
                YodaDocument::AddZoneWithIdToWorld(document, zone_id_6);
            }
        }
        --v194;
        if ( --HIDWORD(v192) > 0 )
            continue;
        break;
    }
    
    
LABEL_296:
    y_5 = 0;
    worldThing = (WorldThing *)((char *)&document->world_things_1[0] + 8);
    while ( 2 )
    {
        x_5 = 0;
        while ( 2 )
        {
            did_not_place_zone = 0;
            document->field_3394 = -1;
            document->field_3390 = -1;
            document->wg_item_id = -1;
            document->wg_last_added_item_id? = -1;
            if ( (_WORD)zone_2 && (_WORD)zone_2 != 305 && !document->world_zones[0] )
            {
                zone_id_1 = -1;
                v193 = (_DWORD *)(__int16)zone_2;
                if ( (unsigned int)((__int16)zone_2 - 201) <= 103 )
                {
                    switch ( (__int16)zone_2 )
                    {
                        case IMWT_SPACEPORT:
                            distance_6 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_1 = YodaDocument::GetZoneIdWithType(document, Town, -1, -1, -1, -1, distance_6, v166);
                            if ( zone_id_1 >= 0 )
                                worldThing->vtable = (void *)Town;
                            goto did_not_find_zone_with_required_type;
                        case IMWT_BLOCKADE_WEST:
                            distance_7 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_1 = YodaDocument::GetZoneIdWithType(document, BlockadeWest, -1, -1, -1, -1, distance_7, v168);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->vtable = (void *)BlockadeWest;
                            goto LABEL_332;
                        case IMWT_BLOCKADE_EAST:
                            distance_8 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_1 = YodaDocument::GetZoneIdWithType(document, BlockadeEast, -1, -1, -1, -1, distance_8, v170);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->vtable = (void *)BlockadeEast;
                            goto LABEL_332;
                        case IMWT_BLOCKADE_NORTH:
                            distance_9 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_1 = YodaDocument::GetZoneIdWithType(document, BlockadeNorth, -1, -1, -1, -1, distance_9, v172);
                            if ( zone_id_1 < 0 )
                                goto did_not_find_zone_with_required_type;
                            worldThing->vtable = (void *)BlockadeNorth;
                            goto LABEL_332;
                        case IMWT_BLOCKATE_SOUTH:
                            distance_16 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_1 = YodaDocument::GetZoneIdWithType(document, BlockadeSouth, -1, -1, -1, -1, distance_16, v174);
                            if ( zone_id_1 >= 0 )
                            {
                                worldThing->vtable = (void *)BlockadeSouth;
                            LABEL_332:
                                LOWORD(worldThing->zone_type) = document->wg_item_id;
                            }
                        did_not_find_zone_with_required_type:
                            did_not_place_zone = 1;
                            break;
                        case 0xCA:
                        case IMWT_SPACEPORT|0x2:
                        case 0xCC:
                        case IMWT_SPACEPORT|0x4:
                        case 0xCE:
                        case IMWT_SPACEPORT|0x6:
                        case 0xD0:
                        case 0xD1:
                        case 0xD2:
                        case 0xD3:
                        case 0xD4:
                        case 0xD5:
                        case 0xD6:
                        case 0xD7:
                        case 0xD8:
                        case IMWT_SPACEPORT|0x10:
                        case 0xDA:
                        case IMWT_SPACEPORT|0x12:
                        case 0xDC:
                        case IMWT_SPACEPORT|0x14:
                        case 0xDE:
                        case IMWT_SPACEPORT|0x16:
                        case 0xE0:
                        case 0xE1:
                        case 0xE2:
                        case 0xE3:
                        case 0xE4:
                        case 0xE5:
                        case 0xE6:
                        case 0xE7:
                        case 0xE8:
                        case IMWT_SPACEPORT|0x20:
                        case 0xEA:
                        case IMWT_SPACEPORT|0x22:
                        case 0xEC:
                        case IMWT_SPACEPORT|0x24:
                        case 0xEE:
                        case IMWT_SPACEPORT|0x26:
                        case 0xF0:
                        case 0xF1:
                        case 0xF2:
                        case 0xF3:
                        case 0xF4:
                        case 0xF5:
                        case 0xF6:
                        case 0xF7:
                        case 0xF8:
                        case IMWT_SPACEPORT|0x30:
                        case 0xFA:
                        case IMWT_SPACEPORT|0x32:
                        case 0xFC:
                        case IMWT_SPACEPORT|0x34:
                        case 0xFE:
                        case 0xFF:
                        case 0x100:
                        case 0x101:
                        case 0x102:
                        case 0x103:
                        case 0x104:
                        case 0x105:
                        case 0x106:
                        case 0x107:
                        case 0x108:
                        case 0x109:
                        case 0x10A:
                        case 0x10B:
                        case 0x10C:
                        case 0x10D:
                        case 0x10E:
                        case 0x10F:
                        case 0x110:
                        case 0x111:
                        case 0x112:
                        case 0x113:
                        case 0x114:
                        case 0x115:
                        case 0x116:
                        case 0x117:
                        case 0x118:
                        case 0x119:
                        case 0x11A:
                        case 0x11B:
                        case 0x11C:
                        case 0x11D:
                        case 0x11E:
                        case 0x11F:
                        case 0x120:
                        case 0x121:
                        case 0x122:
                        case 0x123:
                        case 0x124:
                        case 0x125:
                        case 0x126:
                        case 0x127:
                        case 0x128:
                        case 0x129:
                        case 0x12A:
                        case 0x12B:
                        case 0x12C:
                            break;
                    }
                }
                if ( zone_id_1 < 0 )
                {
                    if ( (_DWORD)v192 != 1 && (_DWORD)v192 != 104 && (_DWORD)v192 != 300 )
                    {
                        if ( did_not_place_zone )
                        {
                            distance_10 = Map::GetDistanceToCenter(x_5, y_5);
                            zone_id_2 = YodaDocument::GetZoneIdWithType(document, EMPTY, -1, -1, -1, -1, distance_10, v176);
                            if ( zone_id_2 >= 0 )
                            {
                                zone_id_7 = zone_id_2;
                                *(_DWORD *)v192 = document->zones.ptrs[zone_id_2];
                                worldThing[-1].field_30 = zone_id_2;
                                worldThing->vtable = (void *)EMPTY;
                                worldThing->field_C = -1;
                                LOWORD(worldThing->zone_type) = -1;
                                worldThing->required_item_id = -1;
                                goto add_zone_to_world;
                            }
                        }
                    }
                }
                else
                {
                    *v193 = document->zones.ptrs[zone_id_1];
                    worldThing[-1].field_30 = zone_id_1;
                    worldThing->field_C = -1;
                    if ( worldThing->vtable != (void *)Town )
                        add_zone_to_world:
                        YodaDocument::AddZoneWithIdToWorld(document, zone_id_7);
                }
            }
            ++worldThing;
            ++x_5;
            LODWORD(v192) = v192 + 4;
            v193 = (_DWORD *)((char *)v193 + 2);
            if ( x_5 < 10 )
                continue;
            break;
        }
        if ( ++y_5 < 10 )
            continue;
        break;
    }
    
    
    if ( !YodaDocument::Unknown_5(document, (int)&zone_id_11) )
    {
        CWordArray::SetSize(&document->puzzle_ids_1, 0, -1);
        CWordArray::SetSize(&document->puzzle_ids_2, 0, -1);
        CWordArray::SetSize(&document->item_ids, 0, -1);
        planet_4 = document->planet;
        if ( planet_4 == TATOOINE )
        {
            puzzle_array_1 = &document->tatooine_puzzle_ids;
        }
        else if ( planet_4 == HOTH )
        {
            puzzle_array_1 = &document->hoth_puzzle_ids;
        }
        else
        {
            if ( planet_4 != ENDOR )
            {
            clean_up_and_return:
                v180 = document->provided_quests.count;
                if ( v180 > 0 )
                {
                    v181 = 0;
                    do
                    {
                        v182 = document->provided_quests.ptrs[v181];
                        if ( v182 )
                            ((void (__stdcall *)(signed int))v182->vtable->Deallocate)(1);
                        ++v181;
                        --v180;
                    }
                    while ( v180 );
                }
                CObArray::SetSize((Array *)&document->provided_quests, 0, -1);
                count = document->required_quests.count;
                if ( count > 0 )
                {
                    idx = 0;
                    do
                    {
                        v185 = document->required_quests.ptrs[idx];
                        if ( v185 )
                            ((void (__stdcall *)(signed int))v185->vtable->Deallocate)(1);
                        ++idx;
                        --count;
                    }
                    while ( count );
                }
                CObArray::SetSize((Array *)&document->required_quests, 0, -1);
                CWordArray::SetSize(&document->puzzle_ids, 0, -1);
                CWordArray::SetSize(&document->world_zone_ids?, 0, -1);
                return 0;
            }
            puzzle_array_1 = &document->endor_puzzle_ids;
        }
        CWordArray::SetSize(puzzle_array_1, 0, -1);
        goto clean_up_and_return;
    }
    
    
    for(int i=0; i < 100; i++) {
        WorldThing *thing = document->world_things[i];
        if(thing && thing.zone_id >= 0) {
            document->GetTileProvidedByZonesHotspots(document, thing.zone_id);
        }
    }
    
    CWordArray::RemoveAt(&document->world_zone_ids?, 0, v206);
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
}
@end
