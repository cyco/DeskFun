//
//  WorldGenerator.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 20/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "WorldGenerator.hpp"

#define Message(_fmt_, ...) do {} while(0)

#define THE_FORCE 0x1ff
#define LOCATOR 0x1a5
#define ADEGAN_CRYSTAL 0x0

WorldGenerator::WorldGenerator(YodaDocument *document) {
    doc = document;
}

bool WorldGenerator::generateRandomWorld(WorldSize size) {
    return this->generateWorld(win_rand(), size, HOTH);
}

bool WorldGenerator::generateWorld(uint16 seed, WorldSize size, Planet planet) {
    doc->planet = planet;
    doc->seed = seed;
    doc->size = (WorldSize)size;
    
    win_srand(seed);
    
    mapGenerator = new MapGenerator((WorldSize)size);
    mapGenerator->generate();
    
    int puzzle_count = mapGenerator->puzzleCount;
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
        goalID = this->GetNewPuzzleId(-1, -1, (ZONE_TYPE)9999, 0);
    }
    
    if(goalID < 0) {
        delete mapGenerator;
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
    if(!placeTransport(doc, mapGenerator->map)) {
        delete mapGenerator;
        return 0;
    }
    
    if(!doLoop0(doc, puzzle_count, puzzles2_count, (uint16*)mapGenerator->puzzles)) return 0;
    
    Message("After Loop 1\n");
    Message("After Loop 1\n");
    if(!loop1(doc, puzzles2_count-1, (uint16*)mapGenerator->puzzles, puzzles1_count - 1, puzzles1_count)){
        delete mapGenerator;
        return 0;
    }
    
    Message("After Loop 2\n");
    if(!loop2(doc, mapGenerator->map)){
        delete mapGenerator;
        return 0;
    }
    
    Message("After Loop 3\n");
    if(!this->Unknown_5((int16*)mapGenerator->map)) {
        delete mapGenerator;
        return doCleanup(doc);
    }
    
    for(int i=0; i < 100; i++) {
        if(doc->world_things[i].zone_id >= 0) {
            doc->GetTileProvidedByZonesHotspots(doc->world_things[i].zone_id);
        }
    }
    doc->chosen_zone_ids.erase(doc->chosen_zone_ids.begin());
    this->RemoveEmptyZoneIdsFromWorld();
    doc->WritePlanetValues();
    
    /*
     doc->contols_puzzle_reuse? = -1;
     doc->world_generation_size = doc->puzzle_ids_2.count + doc->puzzle_ids_1.count + 2 * (y_2 + v204);
     doc->world_generation_time = time(0);
     doc->field_7C = 0;
     */
    Message("End Generate New World\n");
    
    delete mapGenerator;
    return 0;
}

#pragma mark -
int WorldGenerator::placeTransport(YodaDocument *doc, uint16* map) {
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + y * 10;
            
            doc->wg_npc_id = -1;
            doc->wg_item_id = -1;
            doc->wg_last_added_item_id = -1;
            
            if(mapGenerator->map[idx] != MapType_TRAVEL_START)
                continue;
            
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int16 zone_id_8 = this->GetZoneIdWithType(ZONETYPE_TravelStart, -1, -1, -1, -1, distance, 1);
            
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
                if(mapGenerator->map[target_x + y * 10] == MapType_TRAVEL_END && !doc->worldZones[target_x + y * 10]) {
                    target_y = y;
                    foundTravelTarget = true;
                    break;
                }
            }
            
            if (!foundTravelTarget) {
                target_x = 0;
                target_y = 0;
                for(int x=0; x < 10; x++) {
                    if(mapGenerator->map[x + target_y * 10] == MapType_TRAVEL_END && !doc->worldZones[x + target_y * 10]) {
                        target_x = x;
                        foundTravelTarget = true;
                        break;
                    }
                }
                
                if ( !foundTravelTarget ) {
                    target_x = 9;
                    for(int y=0; y < 10; y++) {
                        if(mapGenerator->map[target_x+y*10] == 102 && !doc->worldZones[target_x+y*10]){
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
                    doc->addZoneWithIdToWorld(zone_id_8);
                    doc->addZoneWithIdToWorld(zone_id_9);
                }
                
                continue;
            }
            
            target_y = 9;
            target_x = 0;
            for(int x=0; x < 10; x++) {
                if(mapGenerator->map[x + target_y * 10] == MapType_TRAVEL_END && !doc->worldZones[x + target_y * 10]) {
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
                    doc->addZoneWithIdToWorld(zone_id_8);
                    doc->addZoneWithIdToWorld(zone_id_9);
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

int WorldGenerator::loop2(YodaDocument* doc, uint16* map){
    int v192 = 0;
    
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            MapType zone_2 = (MapType)map[idx];
            int did_not_place_zone = 0;
            doc->field_3394 = -1;
            doc->wg_npc_id = -1;
            doc->wg_item_id = -1;
            doc->wg_last_added_item_id = -1;
            
            WorldThing *worldThing = &doc->world_things[idx];
            if ( zone_2 == MapType_NONE) continue;
            if(zone_2 == MapType_KEPT_FREE) continue;
            if(doc->worldZones[idx] != NULL) continue;
            
            int16 zone_id_1 = doPuzzle(doc, x, y, zone_2, &did_not_place_zone);
            if ( zone_id_1 < 0 ) {
                if (v192 == 1 || v192 == 104 || v192 == 300) continue;
                if (!did_not_place_zone ) continue;
                
                int distance = MapGenerator::GetDistanceToCenter(x, y);
                zone_id_1 = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
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
                doc->addZoneWithIdToWorld(zone_id_1);
            } else {
                worldThing->zone_id = zone_id_1;
                /*
                 *v193 = doc->zones.ptrs[zone_id_1];
                 worldThing->field_C = -1;
                 */
                if ( worldThing->zone_type != ZONETYPE_Town)
                    doc->addZoneWithIdToWorld(zone_id_1);
                    }
        }
    }
    return 1;
}

int WorldGenerator::loop1(YodaDocument* doc, int puzzleMapIdx, uint16* puzzles, int v199, int x_8)
{
    int idx_6 = 0, v195 = 0;
    
    for(int zone_type = puzzleMapIdx; zone_type > 0; zone_type--) {
        doc->wg_zone_type = -1;
        doc->wg_last_added_item_id = -1;
        doc->wg_item_id_unknown_2 = -1;
        doc->wg_item_id = -1;
        doc->wg_item_id_unknown_3 = -1;
        doc->wg_npc_id = -1;
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
                        int distance = MapGenerator::GetDistanceToCenter(x, y);
                        zone_id_5 = this->GetZoneIdWithType(ZONETYPE_Goal, v199-1, -1, idx_6, -1, distance, 0);
                        if ( zone_id_5 < 0 ) {
                            return doCleanup(doc);
                        }
                        
                        doc->wg_zone_type = ZONETYPE_Goal;
                    } else {
                        int rand_2 = win_rand();
                        Message("random = %x\n", rand_2);
                        ZONE_TYPE type = (ZONE_TYPE)((rand_2 & 1) + 15); // was win_rand() & 1
                        
                        // LOWORD(idx_6) = v199 - 1;
                        // item_id = idx_6 = v199-1;
                        // y_2 = 16 - ((unsigned int)((((unsigned __int64)rand_2 >> 32) ^ abs(rand_2) & 1) - ((unsigned __int64)rand_2 >> 32)) < 1);
                        int distance = MapGenerator::GetDistanceToCenter(x, y);
                        zone_id_5 = this->GetZoneIdWithType(type, zone_type-1, -1, item_1, -1, distance, 0);
                        int break_some_more = 0;
                        if ( zone_id_5 >= 0 ) {
                            doc->wg_zone_type = type;
                            break_some_more = 1;
                        }
                        
                        if (!break_some_more && zone_type == ZONETYPE_Use ) {
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_5 = this->GetZoneIdWithType(ZONETYPE_Trade, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return doCleanup(doc);
                            }
                            
                            doc->wg_zone_type = ZONETYPE_Trade;
                        } else if(!break_some_more) {
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_5 = this->GetZoneIdWithType(ZONETYPE_Use, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return doCleanup(doc);
                            }
                            
                            doc->wg_zone_type = ZONETYPE_Use;
                        }
                    }
                    if ( zone_id_5 < 0 ) {
                        return doCleanup(doc);
                    }
                    
                    doc->field_3394 = -1;
                    doc->addZoneWithIdToWorld(zone_id_5);
                    
                    ++idx_6;
                    int idx_5 = x + 10 * y;
                    v195 = 1;
                    doc->world_things[idx_5].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                    doc->world_things[idx_5].zone_id = zone_id_5;
                    doc->world_things[idx_5].findItemID = doc->wg_last_added_item_id;
                    doc->world_things[idx_5].unknown606 = doc->field_3394;
                    doc->world_things[idx_5].requiredItemID = doc->wg_item_id;
                    doc->world_things[idx_5].unknown612 = doc->wg_npc_id;
                    // v147 = (char *)doc + 52 * idx_5;
                    /*
                     *((_WORD *)v147 + 624) = 0;
                     */
                    doc->worldZones[idx_5] = doc->zones[zone_id_5];
                    
                    if ( idx_6 > 200) { breakYLoop=1; break;};
                }
            }
        }
        
        if ( !v195 ) {
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
            if ( zone_id >= 0 ) {
                int world_idx_2 = x + 10 * y;
                doc->world_things[world_idx_2].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                doc->worldZones[world_idx_2] = doc->zones[zone_id];
                doc->world_things[world_idx_2].zone_id = zone_id;
                doc->addZoneWithIdToWorld(zone_id);
            }
        }
    }
    return 1;
}

int WorldGenerator::doCleanup(YodaDocument* doc)
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

int WorldGenerator::doPuzzle(YodaDocument *doc, int x, int y, MapType zone_2, int* did_not_place_zone)
{
    int distance = MapGenerator::GetDistanceToCenter(x, y);
    WorldThing *worldThing = &doc->world_things[x+10*y];
    ZONE_TYPE type = zoneTypeForWorldItem(zone_2);
    
    *did_not_place_zone = 0;
    
    if(type == ZONETYPE_Town) {
        int result = this->GetZoneIdWithType(type, -1, -1, -1, -1, distance, 0);
        if(result >= 0) worldThing->zone_type = ZONETYPE_Town;
            
            *did_not_place_zone = 1;
            return result;
    }
    
    if(ZONETYPE_BlockadeNorth <= type && type <=  ZONETYPE_BlockadeWest) {
        int result = this->GetZoneIdWithType(type, -1, -1, -1, -1, distance, 0);
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

ZONE_TYPE WorldGenerator::zoneTypeForWorldItem(MapType item) {
    switch(item){
        case MapType_SPACEPORT: return ZONETYPE_Town;
        case MapType_BLOCK_WEST: return ZONETYPE_BlockadeWest;
        case MapType_BLOCK_EAST: return ZONETYPE_BlockadeEast;
        case MapType_BLOCK_NORTH: return ZONETYPE_BlockadeNorth;
        case MapType_BLOCK_SOUTH: return ZONETYPE_BlockadeSouth;
        default: return ZONETYPE_Empty;
    }
}

int WorldGenerator::doLoop0(YodaDocument *doc, const int puzzle_count, const int puzzles2_count, const uint16* puzzles)
{
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
            doc->wg_npc_id = -1;
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
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_3 = this->GetZoneIdWithType(ZONETYPE_Goal, zone_id_11 - 1, puzzles2_count-1, item_1, item_2, distance, 1);
                            if ( zone_id_3 < 0 ) break;
                            
                            doc->wg_zone_type = ZONETYPE_Goal;
                            doc->field_3394 = world_puz_idx - 1;
                            // doc->field_3398 = (int)&x_2[-1].field_E + 1;
                        } else {
                            int random = win_rand();
                            Message("random = %x\n", random);
                            int type = ((random ^ 1) & 1) + 15; // was win_rand() & 1
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_3 = this->GetZoneIdWithType((ZONE_TYPE)type,
                                                               zone_id_11 - 1,
                                                               -1,
                                                               item_1,
                                                               -1,
                                                               distance,
                                                               0+1);
                            if ( zone_id_3 < 0) {
                                if ( zone_id_10 == ZONETYPE_Use ) {
                                    int distance = MapGenerator::GetDistanceToCenter(x, y);
                                    zone_id_3 = this->GetZoneIdWithType(ZONETYPE_Trade, v204, -1, ZONETYPE_Use, -1, distance, 0);
                                    if ( zone_id_3 < 0 ) break;
                                    doc->wg_zone_type = ZONETYPE_Trade;
                                } else {
                                    int distance = MapGenerator::GetDistanceToCenter(x, y);
                                    zone_id_3 = this->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance, 0);
                                    if ( zone_id_3 < 0 ) break;
                                    doc->wg_zone_type = ZONETYPE_Use;
                                }
                                doc->field_3394 = world_puz_idx - 1;
                            } else {
                                doc->wg_zone_type = type;
                                doc->field_3394 = world_puz_idx - 1;
                            }
                        }
                        doc->addZoneWithIdToWorld(zone_id_3);
                        if ( zone_id_3 < 0 ) break;
                        int world_idx_1 = x + 10 * y;
                        v204 = 1;
                        
                        doc->world_things[world_idx_1].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                        doc->world_things[world_idx_1].zone_id = zone_id_3;
                        doc->world_things[world_idx_1].findItemID = doc->wg_last_added_item_id;
                        doc->world_things[world_idx_1].unknown606 = doc->field_3394;
                        doc->world_things[world_idx_1].requiredItemID = doc->wg_item_id;
                        doc->world_things[world_idx_1].unknown612 = doc->wg_npc_id;
                        
                        /*
                         *((_WORD *)v113 + 611) = doc->wg_item_id_unknown_2;
                         *((_WORD *)v113 + 609) = doc->wg_item_id_unknown_3;
                         *((_WORD *)v113 + 607) = doc->field_3398;
                         *((_WORD *)v113 + 624) = 1;
                         */
                        doc->worldZones[world_idx_1] = doc->zones[zone_id_3];
                        
                        Message("y_2 = %d\n", zone_id_11);
                        if ( zone_id_11 == 1) { // esp = 0007F51C, y_2 = esp+14 == 0x7F530
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            doc->AddProvidedQuestWithItemID(doc->wg_item_id, distance);
                            
                            Message("v206 = %d\n", x_4);
                            breakAgain = 1;
                            break;
                        }
                        
                        Message("v206 = %d\n", x_4);
                        if(v206 > 200) { breakAgain=1; break; };
                    }
                    
                    if(breakAgain) break;
                    
                    return doCleanup(doc);
                }
                
            do_the_continue_thing:
                row++;
                ++y_1;
                
                y++;
                x = 0;
            }
            while ( row < 100); // (int16 *)&v230;
            Message("x_4 = %d\n", x_4);
            if ( !x_4 ) {
                int distance_1 = MapGenerator::GetDistanceToCenter(x, y);
                int zone_id_4 = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance_1, 0);
                if ( zone_id_4 >= 0 )
                {
                    int idx_4 = x + 10 * y;
                    doc->world_things[idx_4].zone_type = (ZONE_TYPE)doc->wg_zone_type;
                    doc->worldZones[idx_4] = doc->zones[zone_id_4];
                    doc->world_things[idx_4].zone_id = zone_id_4;
                    doc->addZoneWithIdToWorld(zone_id_4);
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

#pragma mark - from Document - 
void WorldGenerator::RemoveEmptyZoneIdsFromWorld(){
    Message("YodaDocument::RemoveEmptyZoneIdsFromWorld()\n");
    vector<int16> non_empty_zone_ids;
    for(uint16 zoneID : this->doc->chosen_zone_ids) {
        if(this->doc->zones[zoneID]->getType() != ZONETYPE_Empty)
            non_empty_zone_ids.push_back(zoneID);
    }
    
    this->doc->chosen_zone_ids.clear();
    
    for(uint16 zoneID : non_empty_zone_ids) {
        this->doc->chosen_zone_ids.push_back(zoneID);
    }
}

int16 WorldGenerator::GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    Message("YodaDocument::GetNewPuzzleId(%d, %d, %d, %d)\n", item_id, (uint16)a3, zone_type, a5);
    int16 puzzle_id;
    Puzzle *puzzle_1;
    int break_from_loop = 0;
    
    vector<int16> puzzle_ids;
    this->doc->GetPuzzleCandidates(puzzle_ids, item_id, a3, zone_type, a5);
    if (puzzle_ids.size() == 0 ) {
        return -1;
    }
    
    this->doc->ShuffleVector(puzzle_ids);
    
    size_t count = puzzle_ids.size();
    int16 puzzle_idx = 0;
    while (1) {
        puzzle_id = puzzle_ids[puzzle_idx];
        puzzle_1 = this->doc->puzzles[puzzle_id];
        if ( this->doc->ContainsPuzzleId(puzzle_id) ) {
            if ( !break_from_loop ) {
                ++puzzle_idx;
                if ( puzzle_idx >= count )
                    ++break_from_loop;
                if ( !break_from_loop )
                    continue;
            } else { return -1; }
        }
        if ( zone_type <= ZONETYPE_Trade ) {
            if ( zone_type == ZONETYPE_Trade ) {
                if ( puzzle_1->type == PuzzleTypeTrade && puzzle_1->questItemIDs[0] == item_id ) {
                    Message("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
                    return puzzle_ids[puzzle_idx];
                }
            } else if ( zone_type == ZONETYPE_Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->questItemIDs[0] == item_id ) {
                Message("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
                return puzzle_ids[puzzle_idx];
            }
            if ( !break_from_loop ) {
                ++puzzle_idx;
                if ( puzzle_idx >= count )
                    ++break_from_loop;
                if ( !break_from_loop )
                    continue;
            } else return -1;
        }
        if ( zone_type != ZONETYPE_Use )
            break;
        
        if ( puzzle_1->type == PuzzleTypeU1 && puzzle_1->questItemIDs[0] == item_id ) {
            Message("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
            return puzzle_ids[puzzle_idx];
        }
        if ( !break_from_loop ) {
            ++puzzle_idx;
            if ( puzzle_idx >= count )
                ++break_from_loop;
            if ( !break_from_loop )
                continue;
        } else return -1;
        
        Message("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", -1, -1);
        return -1;
    }
    
    if ( zone_type != 9999 || puzzle_1->type != PuzzleTypeEnd ){
        if ( !break_from_loop ) {
            ++puzzle_idx;
            if ( puzzle_idx >= count )
                ++break_from_loop;
            if ( !break_from_loop )
                printf("IF THIS EVER HAPPENS WE'D HAVE TO JUMP BACK INTO THE LOOP AND CONTINUE\n");
        } else return -1;
    }
    
    Message("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
    return puzzle_ids[puzzle_idx];
}

int16 WorldGenerator::GetZoneIdWithType(ZONE_TYPE type_1, int a3, int a4, int item_id_1, int item_id_2, int16 item_id_3, int a8) {
    Message("YodaDocument::GetZoneIdWithType(%d, %d, %d, %d, %d, %d, %d)\n", type_1, (uint16)a3, (uint16)a4, (uint16)item_id_1, (uint16)item_id_2, item_id_3, a8);
    
    // item_id_1 = first required quest->itemID, last required quest->itemID
    vector<int16> usable_zone_ids;
    for(int zone_index=0; zone_index < this->doc->zones.size(); zone_index++) {
        Zone *zone = this->doc->zones[zone_index];
        if ( (size_t)zone == -1 || !zone || zone->getPlanet() != this->doc->planet )
            continue;
        
        switch ( type_1 ) {
            case ZONETYPE_Empty:
            case ZONETYPE_BlockadeNorth:
            case ZONETYPE_BlockadeSouth:
            case ZONETYPE_BlockadeEast:
            case ZONETYPE_BlockadeWest:
            case ZONETYPE_TravelStart:
            case ZONETYPE_TravelEnd:
            case ZONETYPE_Goal:
            case ZONETYPE_Town:
            case ZONETYPE_Trade:
            case ZONETYPE_Use:
                if ( zone->getType() == type_1 )
                    usable_zone_ids.push_back(zone_index);
                break;
            case ZONETYPE_Find:
            case ZONETYPE_FindTheForce: {// ?
                if ( zone->getType() == ZONETYPE_Find || zone->getType() == ZONETYPE_FindTheForce )
                    usable_zone_ids.push_back(zone_index);
                break;
            }
            default:
                break;
        }
        
    }
    
    if (usable_zone_ids.size() == 0) {
        return -1;
    }
    
    this->doc->ShuffleVector(usable_zone_ids);
    
    for(int idx=0; idx < usable_zone_ids.size(); idx++) {
        Zone *zone_1; // esi@17
        int count_1 = 0; // edx@24
        int16 v20 = 0; // eax@50
        int v21 = 0; // eax@52
        int v22 = 0; // esi@52
        int puzzle_id = 0; // eax@54
        int v24 = 0; // esi@58
        int v25 = 0; // eax@65
        int zone_type = 0; // eax@80
        unsigned int v33 = 0; // [sp+20h] [bp-2Ch]@58
        int16 item_id[2]; // [sp+28h] [bp-24h]@50
        int16 item_ids = 0; // [sp+2Ch] [bp-20h]@50
        int16 v38 = 0; // [sp+34h] [bp-18h]@54
        
        int zone_id = usable_zone_ids[idx];
        zone_1 = this->doc->zones[zone_id];
        if (this->doc->worldContainsZoneId(zone_id) && (type_1 != ZONETYPE_Goal || this->doc->puzzles_can_be_reused <= 0) )
            continue;
        
        switch (type_1) {
            case ZONETYPE_Empty:
                if ( this->doc->field_2E64 ) {
                    count_1 = (int)zone_1->_hotspots.size();
                    if ( count_1 <= 0 ) {
                        return zone_id;
                    }
                    int idx_1 = 0;
                    int hotspot_idx = 0;
                    do {
                        if ( (zone_1->_hotspots[hotspot_idx])->type == Teleporter )
                            break;
                        ++hotspot_idx;
                        ++idx_1;
                    }
                    while ( idx_1 < count_1 );
                } else if ( zone_1->getType() == ZONETYPE_Empty ) {
                    return zone_id;
                }
                continue;
            case ZONETYPE_BlockadeNorth:
                if ( zone_1->getType() != ZONETYPE_BlockadeNorth
                    || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                return zone_id;
            case ZONETYPE_BlockadeSouth:
                if ( zone_1->getType() != ZONETYPE_BlockadeSouth
                    || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                return zone_id;
            case ZONETYPE_BlockadeEast:
                if ( zone_1->getType() != ZONETYPE_BlockadeEast || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_BlockadeWest:
                if ( zone_1->getType() != ZONETYPE_BlockadeWest || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_TravelStart:
                if ( zone_1->getType() != ZONETYPE_TravelStart || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_TravelEnd:
                if ( zone_1->getType() != ZONETYPE_TravelEnd || this->doc->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_Goal:
                if ( zone_1->getType() != ZONETYPE_Goal )
                    continue;
                if ( this->doc->ZoneHasProvidedItem(zone_id, item_id_1) != 1 )
                    continue;
                if ( this->doc->ZoneHasProvidedItem(zone_id, item_id_2) != 1 )
                    continue;
                
                item_id[0] = this->doc->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                v20 = this->doc->GetItemIDThatsNotRequiredYet(zone_id, a4, 1);
                item_ids = v20;
                if ( item_id[0] < 0 || (signed int)v20 < 0 )
                    continue;
                v21 = GetNewPuzzleId(item_id[0], item_id_1, ZONETYPE_Goal, !a3);
                v22 = v21;
                if ( v21 >= 0 ) this->doc->puzzle_ids.push_back(v21);
                puzzle_id = GetNewPuzzleId((int16)item_ids, item_id_2, ZONETYPE_Goal, !a3);
                v38 = puzzle_id;
                if ( puzzle_id >= 0 ) {
                    this->doc->puzzle_ids.push_back(puzzle_id);
                }
                if ( v22 < 0 || v38 < 0 )
                    continue;
                
                this->doc->puzzle_ids_1[(int16)a3] = v22;
                v33 = 2 * (int16)a3;
                v24 = (int16)a4;
                this->doc->puzzle_ids_2[v24] = v38;
                if ( this->doc->Unknown_7(zone_id, a3, a4, item_id_3, a8) != 1 ) {
                    this->doc->puzzle_ids_1[v33 / 2] = -1;
                    this->doc->puzzle_ids_2[v24] = -1;
                    continue;
                }
                this->doc->AddRequiredQuestWithItemID(item_id[0], item_id_3);
                this->doc->AddRequiredQuestWithItemID((int16)item_ids, item_id_3);
                return zone_id;
            case ZONETYPE_Town:
                if ( zone_1->getType() != 11 ) continue;
                return zone_id;
            case ZONETYPE_Trade:
                if ( zone_1->getType() != 15 ) continue;
                if ( this->doc->ZoneHasProvidedItem(zone_id, item_id_1) != 1 ) continue;
                v38 = this->doc->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( v38 < 0 ) continue;
                v25 = GetNewPuzzleId(v38, item_id_1, ZONETYPE_Trade, !a3);
                if ( v25 < 0 ) continue;
                
                if(a8) this->doc->puzzle_ids_1[a3] = v25;
                else   this->doc->puzzle_ids_2[a3] = v25;
                
                if (this->doc->Unknown_1(zone_id, a3, item_id_3, a8) != 1) continue;
                
                this->doc->puzzle_ids.push_back(v25);
                this->doc->AddRequiredQuestWithItemID(v38, item_id_3);
                
                return zone_id;
            case ZONETYPE_Use:
            {
                if ( zone_1->getType() != 16 )
                    continue;
                if ( this->doc->ZoneHasProvidedItem(zone_id, item_id_1) != 1 )
                    continue;
                int16 puzzleID1 = this->doc->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( puzzleID1 < 0 ) continue;
                
                int16 puzzleID2 = this->GetNewPuzzleId(puzzleID1, item_id_1, ZONETYPE_Use, !a3);
                if ( puzzleID2 < 0 ) continue;
                
                if(a8) this->doc->puzzle_ids_1[a3] = puzzleID2;
                else this->doc->puzzle_ids_2[a3] = puzzleID2;
                
                if ( this->doc->use_ids_from_array_1(zone_id, a3, item_id_3, a8) != 1 ) continue;
                this->doc->puzzle_ids.push_back(puzzleID2);
                
                this->doc->AddRequiredQuestWithItemID(puzzleID1, item_id_3);
                return zone_id;
            }
            case ZONETYPE_Find:
                zone_type = zone_1->getType();
                if ( (zone_type != ZONETYPE_Find && zone_type != ZONETYPE_FindTheForce)
                    || this->doc->AssociateItemWithZoneHotspot(zone_id, item_id_1, item_id_3) != 1 ) {
                    continue;
                }
                return zone_id;
            default:
                continue;
        }
    }
    
    return -1;
}

int WorldGenerator::Unknown_5(int16* world){
    Message("YodaDocument::Unknown_5(...)\n");
    signed int result = 0;
    int zone_id = 0;
    Zone *zone = NULL;
    WorldSize world_size = this->doc->size;
    int v27 = 0;
    int zone_id_1 = 0;
    int v38 = 0;
    int v39 = 0;
    
    this->doc->AddProvidedQuestWithItemID(THE_FORCE, 2);
    this->doc->AddProvidedQuestWithItemID(LOCATOR, 1);
    
    int x_1 = 0, y_1 = 0;
    // int do_second_part = this->providedItems.size() == 0;
    int do_second_part = true;
    
    for(Quest *quest : this->doc->providedItems) {
        int x = 0, y = 0;
        if ( this->doc->findPlaceToPutPuzzle(quest->unknown, world, &x, &y) != 1 )
            return 0;
        
        int zoneID = this->GetZoneIdWithType(ZONETYPE_Find, -1, -1, quest->itemID, -1, quest->unknown, 0);
        if ( zoneID < 0 ) {
            Message("YodaDocument::Unknown_5 => %d\n", 0);
            return 0;
        }
        
        int idx = x + 10 * y;
        this->doc->world_things[idx].zone_type = ZONETYPE_Find;
        this->doc->world_things[idx].zone_id = zoneID;
        this->doc->world_things[idx].findItemID = this->doc->wg_last_added_item_id;
        this->doc->worldZones[idx] = this->doc->zones[zoneID];
        world[idx] = 306;
        
        this->doc->addZoneWithIdToWorld(zoneID);
    }
    
    if(do_second_part){
        Message("YodaDocument::Unknown_5 -> cleanup\n");
        for(Quest *quest : this->doc->providedItems) {
            delete quest;
        }
        this->doc->providedItems.clear();
        this->doc->item_ids.clear();
        
        v38 = 0;
        zone_id_1 = -1;
        
        int x = 0, y = 0;
        for(y=0; y < 10; y++) {
            for(x=0; x < 10; x++) {
                int intermediate_puzzle_item = world[x + 10 * y];
                if ( intermediate_puzzle_item == 1 || intermediate_puzzle_item == 300 || intermediate_puzzle_item == 104 ) {
                    int distance = MapGenerator::GetDistanceToCenter(x, y);
                    this->doc->field_2E64 = intermediate_puzzle_item == 104 || distance < 2;
                    
                    if ( this->doc->field_2E64 ) {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    } else if (x_1) { // used to be x
                        zone_id = zone_id_1;
                    } else {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    }
                    
                    if ( zone_id < 0 ) {
                        Message("YodaDocument::Unknown_5 => %d\n", 0);
                        return 0;
                    }
                    
                    while(1) {
                        zone = this->doc->zones[zone_id];
                        if ( this->doc->field_2E64 ) break;
                        
                        int has_teleporter = 0;
                        for(Hotspot *hotspot : zone->_hotspots) {
                            if(hotspot->type == Teleporter) {
                                has_teleporter = 1;
                                break;
                            }
                        }
                        
                        if ( !has_teleporter )
                            break;
                        
                        Message("y = %d\n", y_1);
                        if ( !y_1 ) {
                            y_1=1;
                            v39 = x;
                            v38 = y;
                            Message("x = %d\n", x_1);
                            if ( !x_1 ) break;
                            
                            x_1 = 0;
                            zone_id_1 = -1;
                            break;
                        }
                        
                        if ( world_size == WORLD_SIZE_SMALL ) {
                            if ( abs(v39 - x) > 1 || abs(v38 - y) > 1 ) {
                                y_1++;
                                v39 = x;
                                v38 = y;
                                Message("x = %d\n", x_1);
                                if ( !x_1 )
                                    break;
                                
                                x_1 = 0;
                                zone_id_1 = -1;
                                break;
                            }
                        } else if ( world_size == WORLD_SIZE_MEDIUM ) {
                            if ( abs(v39 - x) > 1 || abs(v38 - y) > 1 ) {
                                y_1++;
                                v39 = x;
                                v38 = y;
                                Message("x_1 = %d\n", x_1);
                                if ( !x_1 )
                                    break;
                                
                                x_1 = 0;
                                zone_id_1 = -1;
                                break;
                            }
                        } else {
                            if ( world_size != WORLD_SIZE_LARGE ) break;
                            if ( abs(v39 - x) > 2 || abs(v38 - y) > 2 ) {
                                y_1++;
                                v39 = x;
                                v38 = y;
                                Message("x = %d\n", x_1);
                                if ( !x_1 )
                                    break;
                                
                                x_1 = 0;
                                zone_id_1 = -1;
                                break;
                            }
                        }
                        zone_id_1 = zone_id;
                        x_1 = 1;
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                        if ( zone_id < 0 )  {
                            Message("YodaDocument::Unknown_5 => %d\n", 0);
                            return 0;
                        };
                    }
                    
                    int idx = x + 10 * y;
                    this->doc->worldZones[idx] = zone;
                    this->doc->world_things[idx].zone_id = zone_id;
                    this->doc->world_things[idx].zone_type = ZONETYPE_Empty;
                    this->doc->world_things[idx].unknown606 = -1;
                    this->doc->world_things[idx].requiredItemID = -1;
                    this->doc->world_things[idx].unknown612 = -1;
                    this->doc->world_things[idx].findItemID = -1;
                    
                    this->doc->addZoneWithIdToWorld(zone_id);
                    
                    if ( zone_id == zone_id_1 ) {
                        zone_id_1 = -1;
                        x_1 = 0;
                    }
                }
            }
        }
        
        Message("y = %d\n", y_1);
        if ( y_1 == 1 ) {
            int y = v38;
            int x = v39;
            this->doc->field_2E64 = 1;
            
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, v27);
            if (zone_id != -1) {
                int idx = x + 10 * y;
                this->doc->world_things[idx].zone_type = (ZONE_TYPE)1;
                this->doc->worldZones[idx] = this->doc->zones[zone_id];
                this->doc->world_things[idx].zone_id = zone_id;
                this->doc->world_things[idx].unknown606 = -1;
                this->doc->world_things[idx].requiredItemID = -1;
                this->doc->world_things[idx].findItemID = -1;
                this->doc->addZoneWithIdToWorld(zone_id);
            }
        }
        return 1;
    }
    
    Message("YodaDocument::Unknown_5 => %d\n", result);
    return result;
}
