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

int TILE_SPEC_THE_FORCE  = 0x40;
int TILE_SPEC_USEFUL  = 0x80;
int TILE_SPEC_MAP    = 0x100000;


WorldGenerator::WorldGenerator(YodaDocument *document) {
    doc = document;
    
    worldThings.resize(100);
    for(int i=0; i < 100; i++) {
        worldThings[i].zone_type = (ZONE_TYPE)0;
        worldThings[i].zone_id = -1;
        worldThings[i].findItemID = -1;
        worldThings[i].unknown606 = -1;
        worldThings[i].requiredItemID = -1;
        worldThings[i].unknown612 = -1;
    }
}

WorldGenerator::~WorldGenerator(){
    for(Quest *q : this->providedItemQuests) {
        delete q;
    }
    this->providedItemQuests.clear();
    
    for(Quest *q : this->requiredItemQuests) {
        delete q;
    }
    this->requiredItemQuests.clear();
    
    worldThings.clear();
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
    this->field_68 = 0;
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
    
    this->puzzle_ids_1.clear();
    this->puzzle_ids_1.resize(puzzles1_count+1, -1);
    
    this->puzzle_ids_2.clear();
    this->puzzle_ids_2.resize(puzzles2_count+1, -1);
    
    this->worldZones.clear();
    this->worldZones.resize(100, 0);
    this->puzzle_ids.push_back(goalID);
    this->puzzle_ids_1[puzzles1_count] = goalID;
    this->puzzle_ids_2[puzzles2_count] = goalID;
    
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
        if(this->worldThings[i].zone_id >= 0) {
            this->GetTileProvidedByZonesHotspots(this->worldThings[i].zone_id);
        }
    }
    
    this->chosen_zone_ids.erase(this->chosen_zone_ids.begin());
    this->RemoveEmptyZoneIdsFromWorld();
    this->WritePlanetValues();
    
    /*
     doc->contols_puzzle_reuse? = -1;
     doc->world_generation_size = this->puzzle_ids_2.count + this->puzzle_ids_1.count + 2 * (y_2 + v204);
     doc->world_generation_time = time(0);
     this->field_7C = 0;
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
            
            this->wg_npc_id = -1;
            this->wg_item_id = -1;
            this->wg_last_added_item_id = -1;
            
            if(mapGenerator->map[idx] != MapType_TRAVEL_START)
                continue;
            
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int16 zone_id_8 = this->GetZoneIdWithType(ZONETYPE_TravelStart, -1, -1, -1, -1, distance, 1);
            
            int zone_id_10 = -1;
            if ( zone_id_8 < 0 ) continue;
            
            this->worldThings[idx].zone_id = zone_id_8;
            this->worldThings[idx].zone_type = ZONETYPE_TravelStart;
            this->worldThings[idx].requiredItemID = this->wg_item_id;
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
                if(mapGenerator->map[target_x + y * 10] == MapType_TRAVEL_END && !this->worldZones[target_x + y * 10]) {
                    target_y = y;
                    foundTravelTarget = true;
                    break;
                }
            }
            
            if (!foundTravelTarget) {
                target_x = 0;
                target_y = 0;
                for(int x=0; x < 10; x++) {
                    if(mapGenerator->map[x + target_y * 10] == MapType_TRAVEL_END && !this->worldZones[x + target_y * 10]) {
                        target_x = x;
                        foundTravelTarget = true;
                        break;
                    }
                }
                
                if ( !foundTravelTarget ) {
                    target_x = 9;
                    for(int y=0; y < 10; y++) {
                        if(mapGenerator->map[target_x+y*10] == 102 && !this->worldZones[target_x+y*10]){
                            foundTravelTarget = 1;
                            target_y = y;
                            break;
                        }
                    }
                }
            }
            
            if ( foundTravelTarget ){
                int16 zone_id_9 = zone_id_10;
                if ( !this->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = target_x + 10 * target_y;
                    Message("transport loop: %dx%d\n", target_x, target_y);
                    Message("transport loop: %dx%d\n", target_x, target_y);
                    this->worldZones[idx_3] = doc->zones[zone_id_9];
                    this->worldThings[idx_3].zone_id = zone_id_9;;
                    this->worldThings[idx_3].zone_type = ZONETYPE_TravelEnd;
                    this->worldThings[idx_3].requiredItemID = this->wg_item_id;
                    // v87 = zone_2;
                    // v88 = (char *)doc + 0x34 * idx_3;
                    /*
                     LOWORD(idx_3) = (_WORD)zone_id_8;
                     *(_DWORD *)transport_count = v87;
                     */
                    this->addZoneWithIdToWorld(zone_id_8);
                    this->addZoneWithIdToWorld(zone_id_9);
                }
                
                continue;
            }
            
            target_y = 9;
            target_x = 0;
            for(int x=0; x < 10; x++) {
                if(mapGenerator->map[x + target_y * 10] == MapType_TRAVEL_END && !this->worldZones[x + target_y * 10]) {
                    target_x = x;
                    foundTravelTarget = true;
                }
            }
            
            if ( foundTravelTarget )
            {
                int16 zone_id_9 = zone_id_10;
                if ( !this->worldContainsZoneId(zone_id_10) )
                {
                    int idx_3 = target_x + 10 * target_y;
                    Message("transport loop: %dx%d\n", target_x, target_y);
                    this->worldZones[idx_3] = doc->zones[zone_id_10];
                    this->worldThings[idx_3].zone_id = zone_id_9;
                    this->worldThings[idx_3].zone_type = ZONETYPE_TravelEnd;
                    this->worldThings[idx_3].requiredItemID = this->wg_item_id;
                    // v87 = zone_2;
                    // v88 = (char *)doc + 0x34 * idx_3;
                    /*
                     LOWORD(idx_3) = (_WORD)zone_id_8;
                     *(_DWORD *)transport_count = v87;
                     */
                    this->addZoneWithIdToWorld(zone_id_8);
                    this->addZoneWithIdToWorld(zone_id_9);
                }
            }
            else
            {
                this->RemoveQuestProvidingItem(this->wg_item_id);
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
            this->field_3394 = -1;
            this->wg_npc_id = -1;
            this->wg_item_id = -1;
            this->wg_last_added_item_id = -1;
            
            WorldThing *worldThing = &this->worldThings[idx];
            if ( zone_2 == MapType_NONE) continue;
            if(zone_2 == MapType_KEPT_FREE) continue;
            if(this->worldZones[idx] != NULL) continue;
            
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
                this->addZoneWithIdToWorld(zone_id_1);
            } else {
                worldThing->zone_id = zone_id_1;
                /*
                 *v193 = doc->zones.ptrs[zone_id_1];
                 worldThing->field_C = -1;
                 */
                if ( worldThing->zone_type != ZONETYPE_Town)
                    this->addZoneWithIdToWorld(zone_id_1);
                    }
        }
    }
    return 1;
}

int WorldGenerator::loop1(YodaDocument* doc, int puzzleMapIdx, uint16* puzzles, int v199, int x_8)
{
    int idx_6 = 0, v195 = 0;
    
    for(int zone_type = puzzleMapIdx; zone_type > 0; zone_type--) {
        this->wg_zone_type = -1;
        this->wg_last_added_item_id = -1;
        this->wg_item_id_unknown_2 = -1;
        this->wg_item_id = -1;
        this->wg_item_id_unknown_3 = -1;
        this->wg_npc_id = -1;
        this->field_3394 = -1;
        this->field_3398 = -1;
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
                int zone_2 = this->puzzle_ids_2[zone_type];
                
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
                        
                        this->wg_zone_type = ZONETYPE_Goal;
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
                            this->wg_zone_type = type;
                            break_some_more = 1;
                        }
                        
                        if (!break_some_more && zone_type == ZONETYPE_Use ) {
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_5 = this->GetZoneIdWithType(ZONETYPE_Trade, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return doCleanup(doc);
                            }
                            
                            this->wg_zone_type = ZONETYPE_Trade;
                        } else if(!break_some_more) {
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            zone_id_5 = this->GetZoneIdWithType(ZONETYPE_Use, v195, -1, 0, -1, distance, 0);
                            if ( zone_id_5 < 0 ) {
                                return doCleanup(doc);
                            }
                            
                            this->wg_zone_type = ZONETYPE_Use;
                        }
                    }
                    if ( zone_id_5 < 0 ) {
                        return doCleanup(doc);
                    }
                    
                    this->field_3394 = -1;
                    this->addZoneWithIdToWorld(zone_id_5);
                    
                    ++idx_6;
                    int idx_5 = x + 10 * y;
                    v195 = 1;
                    this->worldThings[idx_5].zone_type = (ZONE_TYPE)this->wg_zone_type;
                    this->worldThings[idx_5].zone_id = zone_id_5;
                    this->worldThings[idx_5].findItemID = this->wg_last_added_item_id;
                    this->worldThings[idx_5].unknown606 = this->field_3394;
                    this->worldThings[idx_5].requiredItemID = this->wg_item_id;
                    this->worldThings[idx_5].unknown612 = this->wg_npc_id;
                    // v147 = (char *)doc + 52 * idx_5;
                    /*
                     *((_WORD *)v147 + 624) = 0;
                     */
                    this->worldZones[idx_5] = doc->zones[zone_id_5];
                    
                    if ( idx_6 > 200) { breakYLoop=1; break;};
                }
            }
        }
        
        if ( !v195 ) {
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
            if ( zone_id >= 0 ) {
                int world_idx_2 = x + 10 * y;
                this->worldThings[world_idx_2].zone_type = (ZONE_TYPE)this->wg_zone_type;
                this->worldZones[world_idx_2] = doc->zones[zone_id];
                this->worldThings[world_idx_2].zone_id = zone_id;
                this->addZoneWithIdToWorld(zone_id);
            }
        }
    }
    return 1;
}

int WorldGenerator::doCleanup(YodaDocument* doc)
{
    this->puzzle_ids_1.clear();
    this->puzzle_ids_2.clear();
    this->item_ids.clear();
    
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
    
    for(Quest *quest : this->providedItemQuests)
        delete quest;
    this->providedItemQuests.clear();
    
    for(Quest *quest : this->requiredItemQuests)
        delete quest;
    this->requiredItemQuests.clear();
    
    this->puzzle_ids.clear();
    this->chosen_zone_ids.clear();
    Message("-= FAILURE 3 =-\n");
    
    return 0;
}

int WorldGenerator::doPuzzle(YodaDocument *doc, int x, int y, MapType zone_2, int* did_not_place_zone)
{
    int distance = MapGenerator::GetDistanceToCenter(x, y);
    WorldThing *worldThing = &this->worldThings[x+10*y];
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
        worldThing->requiredItemID = this->wg_item_id;
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
    int zone_id_11 = (int)this->puzzle_ids_1.size()-1;
    if ( puzzle_count > 0 ){
        do {
            this->wg_zone_type = -1;
            this->wg_item_id = -1;
            this->wg_item_id_unknown_2 = -1;
            this->wg_item_id_unknown_3 = -1;
            this->wg_last_added_item_id = -1;
            this->wg_npc_id = -1;
            this->field_3394 = -1;
            this->field_3398 = -1;
            
            y_1 = 0;
            int row = 0;
            int zone_2 = this->puzzle_ids_1[zone_id_11];
            
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
                            
                            this->wg_zone_type = ZONETYPE_Goal;
                            this->field_3394 = world_puz_idx - 1;
                            // this->field_3398 = (int)&x_2[-1].field_E + 1;
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
                                    this->wg_zone_type = ZONETYPE_Trade;
                                } else {
                                    int distance = MapGenerator::GetDistanceToCenter(x, y);
                                    zone_id_3 = this->GetZoneIdWithType(ZONETYPE_Use, v204, -1, zone_id_10, -1, distance, 0);
                                    if ( zone_id_3 < 0 ) break;
                                    this->wg_zone_type = ZONETYPE_Use;
                                }
                                this->field_3394 = world_puz_idx - 1;
                            } else {
                                this->wg_zone_type = type;
                                this->field_3394 = world_puz_idx - 1;
                            }
                        }
                        this->addZoneWithIdToWorld(zone_id_3);
                        if ( zone_id_3 < 0 ) break;
                        int world_idx_1 = x + 10 * y;
                        v204 = 1;
                        
                        this->worldThings[world_idx_1].zone_type = (ZONE_TYPE)this->wg_zone_type;
                        this->worldThings[world_idx_1].zone_id = zone_id_3;
                        this->worldThings[world_idx_1].findItemID = this->wg_last_added_item_id;
                        this->worldThings[world_idx_1].unknown606 = this->field_3394;
                        this->worldThings[world_idx_1].requiredItemID = this->wg_item_id;
                        this->worldThings[world_idx_1].unknown612 = this->wg_npc_id;
                        
                        /*
                         *((_WORD *)v113 + 611) = this->wg_item_id_unknown_2;
                         *((_WORD *)v113 + 609) = this->wg_item_id_unknown_3;
                         *((_WORD *)v113 + 607) = this->field_3398;
                         *((_WORD *)v113 + 624) = 1;
                         */
                        this->worldZones[world_idx_1] = doc->zones[zone_id_3];
                        
                        Message("y_2 = %d\n", zone_id_11);
                        if ( zone_id_11 == 1) { // esp = 0007F51C, y_2 = esp+14 == 0x7F530
                            int distance = MapGenerator::GetDistanceToCenter(x, y);
                            this->AddProvidedQuestWithItemID(this->wg_item_id, distance);
                            
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
                    this->worldThings[idx_4].zone_type = (ZONE_TYPE)this->wg_zone_type;
                    this->worldZones[idx_4] = doc->zones[zone_id_4];
                    this->worldThings[idx_4].zone_id = zone_id_4;
                    this->addZoneWithIdToWorld(zone_id_4);
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
    Message("WorldGenerator::RemoveEmptyZoneIdsFromWorld()\n");
    vector<int16> non_empty_zone_ids;
    for(uint16 zoneID : this->chosen_zone_ids) {
        if(this->doc->zones[zoneID]->getType() != ZONETYPE_Empty)
            non_empty_zone_ids.push_back(zoneID);
    }
    
    this->chosen_zone_ids.clear();
    
    for(uint16 zoneID : non_empty_zone_ids) {
        this->chosen_zone_ids.push_back(zoneID);
    }
}

int16 WorldGenerator::GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    Message("WorldGenerator::GetNewPuzzleId(%d, %d, %d, %d)\n", item_id, (uint16)a3, zone_type, a5);
    int16 puzzle_id;
    Puzzle *puzzle_1;
    int break_from_loop = 0;
    
    vector<int16> puzzle_ids;
    this->GetPuzzleCandidates(puzzle_ids, item_id, a3, zone_type, a5);
    if (puzzle_ids.size() == 0 ) {
        return -1;
    }
    
    this->ShuffleVector(puzzle_ids);
    
    size_t count = puzzle_ids.size();
    int16 puzzle_idx = 0;
    while (1) {
        puzzle_id = puzzle_ids[puzzle_idx];
        puzzle_1 = this->doc->puzzles[puzzle_id];
        if ( this->ContainsPuzzleId(puzzle_id) ) {
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
                    Message("WorldGenerator::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
                    return puzzle_ids[puzzle_idx];
                }
            } else if ( zone_type == ZONETYPE_Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->questItemIDs[0] == item_id ) {
                Message("WorldGenerator::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
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
            Message("WorldGenerator::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
            return puzzle_ids[puzzle_idx];
        }
        if ( !break_from_loop ) {
            ++puzzle_idx;
            if ( puzzle_idx >= count )
                ++break_from_loop;
            if ( !break_from_loop )
                continue;
        } else return -1;
        
        Message("WorldGenerator::GetNewPuzzleId => 0x%x (%d)\n", -1, -1);
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
    
    Message("WorldGenerator::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
    return puzzle_ids[puzzle_idx];
}

int16 WorldGenerator::GetZoneIdWithType(ZONE_TYPE type_1, int a3, int a4, int item_id_1, int item_id_2, int16 item_id_3, int a8) {
    Message("WorldGenerator::GetZoneIdWithType(%d, %d, %d, %d, %d, %d, %d)\n", type_1, (uint16)a3, (uint16)a4, (uint16)item_id_1, (uint16)item_id_2, item_id_3, a8);
    
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
    
    this->ShuffleVector(usable_zone_ids);
    
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
        if (this->worldContainsZoneId(zone_id) && (type_1 != ZONETYPE_Goal || this->doc->puzzles_can_be_reused <= 0) )
            continue;
        
        switch (type_1) {
            case ZONETYPE_Empty:
                if ( this->field_2E64 ) {
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
                    || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                return zone_id;
            case ZONETYPE_BlockadeSouth:
                if ( zone_1->getType() != ZONETYPE_BlockadeSouth
                    || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                return zone_id;
            case ZONETYPE_BlockadeEast:
                if ( zone_1->getType() != ZONETYPE_BlockadeEast || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_BlockadeWest:
                if ( zone_1->getType() != ZONETYPE_BlockadeWest || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_TravelStart:
                if ( zone_1->getType() != ZONETYPE_TravelStart || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_TravelEnd:
                if ( zone_1->getType() != ZONETYPE_TravelEnd || this->SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                return zone_id;
            case ZONETYPE_Goal:
                if ( zone_1->getType() != ZONETYPE_Goal )
                    continue;
                if ( this->ZoneHasProvidedItem(zone_id, item_id_1) != 1 )
                    continue;
                if ( this->ZoneHasProvidedItem(zone_id, item_id_2) != 1 )
                    continue;
                
                item_id[0] = this->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                v20 = this->GetItemIDThatsNotRequiredYet(zone_id, a4, 1);
                item_ids = v20;
                if ( item_id[0] < 0 || (signed int)v20 < 0 )
                    continue;
                v21 = GetNewPuzzleId(item_id[0], item_id_1, ZONETYPE_Goal, !a3);
                v22 = v21;
                if ( v21 >= 0 ) this->puzzle_ids.push_back(v21);
                puzzle_id = GetNewPuzzleId((int16)item_ids, item_id_2, ZONETYPE_Goal, !a3);
                v38 = puzzle_id;
                if ( puzzle_id >= 0 ) {
                    this->puzzle_ids.push_back(puzzle_id);
                }
                if ( v22 < 0 || v38 < 0 )
                    continue;
                
                this->puzzle_ids_1[(int16)a3] = v22;
                v33 = 2 * (int16)a3;
                v24 = (int16)a4;
                this->puzzle_ids_2[v24] = v38;
                if ( this->Unknown_7(zone_id, a3, a4, item_id_3, a8) != 1 ) {
                    this->puzzle_ids_1[v33 / 2] = -1;
                    this->puzzle_ids_2[v24] = -1;
                    continue;
                }
                this->AddRequiredQuestWithItemID(item_id[0], item_id_3);
                this->AddRequiredQuestWithItemID((int16)item_ids, item_id_3);
                return zone_id;
            case ZONETYPE_Town:
                if ( zone_1->getType() != 11 ) continue;
                return zone_id;
            case ZONETYPE_Trade:
                if ( zone_1->getType() != 15 ) continue;
                if ( this->ZoneHasProvidedItem(zone_id, item_id_1) != 1 ) continue;
                v38 = this->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( v38 < 0 ) continue;
                v25 = GetNewPuzzleId(v38, item_id_1, ZONETYPE_Trade, !a3);
                if ( v25 < 0 ) continue;
                
                if(a8) this->puzzle_ids_1[a3] = v25;
                else   this->puzzle_ids_2[a3] = v25;
                
                if (this->Unknown_1(zone_id, a3, item_id_3, a8) != 1) continue;
                
                this->puzzle_ids.push_back(v25);
                this->AddRequiredQuestWithItemID(v38, item_id_3);
                
                return zone_id;
            case ZONETYPE_Use:
            {
                if ( zone_1->getType() != 16 )
                    continue;
                if ( this->ZoneHasProvidedItem(zone_id, item_id_1) != 1 )
                    continue;
                int16 puzzleID1 = this->GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( puzzleID1 < 0 ) continue;
                
                int16 puzzleID2 = this->GetNewPuzzleId(puzzleID1, item_id_1, ZONETYPE_Use, !a3);
                if ( puzzleID2 < 0 ) continue;
                
                if(a8) this->puzzle_ids_1[a3] = puzzleID2;
                else this->puzzle_ids_2[a3] = puzzleID2;
                
                if ( this->use_ids_from_array_1(zone_id, a3, item_id_3, a8) != 1 ) continue;
                this->puzzle_ids.push_back(puzzleID2);
                
                this->AddRequiredQuestWithItemID(puzzleID1, item_id_3);
                return zone_id;
            }
            case ZONETYPE_Find:
                zone_type = zone_1->getType();
                if ( (zone_type != ZONETYPE_Find && zone_type != ZONETYPE_FindTheForce)
                    || this->AssociateItemWithZoneHotspot(zone_id, item_id_1, item_id_3) != 1 ) {
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
    Message("WorldGenerator::Unknown_5(...)\n");
    signed int result = 0;
    int zone_id = 0;
    Zone *zone = NULL;
    WorldSize world_size = this->doc->size;
    int v27 = 0;
    int zone_id_1 = 0;
    int v38 = 0;
    int v39 = 0;
    
    this->AddProvidedQuestWithItemID(THE_FORCE, 2);
    this->AddProvidedQuestWithItemID(LOCATOR, 1);
    
    int x_1 = 0, y_1 = 0;
    // int do_second_part = this->providedItemQuests.size() == 0;
    int do_second_part = true;
    
    for(Quest *quest : this->providedItemQuests) {
        int x = 0, y = 0;
        if ( this->findPlaceToPutPuzzle(quest->unknown, world, &x, &y) != 1 )
            return 0;
        
        int zoneID = this->GetZoneIdWithType(ZONETYPE_Find, -1, -1, quest->itemID, -1, quest->unknown, 0);
        if ( zoneID < 0 ) {
            Message("WorldGenerator::Unknown_5 => %d\n", 0);
            return 0;
        }
        
        int idx = x + 10 * y;
        this->worldThings[idx].zone_type = ZONETYPE_Find;
        this->worldThings[idx].zone_id = zoneID;
        this->worldThings[idx].findItemID = this->wg_last_added_item_id;
        this->worldZones[idx] = this->doc->zones[zoneID];
        world[idx] = 306;
        
        this->addZoneWithIdToWorld(zoneID);
    }
    
    if(do_second_part){
        Message("WorldGenerator::Unknown_5 -> cleanup\n");
        for(Quest *quest : this->providedItemQuests) {
            delete quest;
        }
        this->providedItemQuests.clear();
        this->item_ids.clear();
        
        v38 = 0;
        zone_id_1 = -1;
        
        int x = 0, y = 0;
        for(y=0; y < 10; y++) {
            for(x=0; x < 10; x++) {
                int intermediate_puzzle_item = world[x + 10 * y];
                if ( intermediate_puzzle_item == 1 || intermediate_puzzle_item == 300 || intermediate_puzzle_item == 104 ) {
                    int distance = MapGenerator::GetDistanceToCenter(x, y);
                    this->field_2E64 = intermediate_puzzle_item == 104 || distance < 2;
                    
                    if ( this->field_2E64 ) {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    } else if (x_1) { // used to be x
                        zone_id = zone_id_1;
                    } else {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    }
                    
                    if ( zone_id < 0 ) {
                        Message("WorldGenerator::Unknown_5 => %d\n", 0);
                        return 0;
                    }
                    
                    while(1) {
                        zone = this->doc->zones[zone_id];
                        if ( this->field_2E64 ) break;
                        
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
                            Message("WorldGenerator::Unknown_5 => %d\n", 0);
                            return 0;
                        };
                    }
                    
                    int idx = x + 10 * y;
                    this->worldZones[idx] = zone;
                    this->worldThings[idx].zone_id = zone_id;
                    this->worldThings[idx].zone_type = ZONETYPE_Empty;
                    this->worldThings[idx].unknown606 = -1;
                    this->worldThings[idx].requiredItemID = -1;
                    this->worldThings[idx].unknown612 = -1;
                    this->worldThings[idx].findItemID = -1;
                    
                    this->addZoneWithIdToWorld(zone_id);
                    
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
            this->field_2E64 = 1;
            
            int distance = MapGenerator::GetDistanceToCenter(x, y);
            int zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, v27);
            if (zone_id != -1) {
                int idx = x + 10 * y;
                this->worldThings[idx].zone_type = (ZONE_TYPE)1;
                this->worldZones[idx] = this->doc->zones[zone_id];
                this->worldThings[idx].zone_id = zone_id;
                this->worldThings[idx].unknown606 = -1;
                this->worldThings[idx].requiredItemID = -1;
                this->worldThings[idx].findItemID = -1;
                this->addZoneWithIdToWorld(zone_id);
            }
        }
        return 1;
    }
    
    Message("WorldGenerator::Unknown_5 => %d\n", result);
    return result;
}


signed int WorldGenerator::Unknown_1(int16 zone_id, int16 a3, int16 distance, int16 a8) {
    Message("WorldGenerator::Unknown_1(%d, %d, %d, %d)\n", zone_id, a3, distance, a8);
    if(false) return 0x10;
    if(false) return 0x10;
    
    if(false == 0xF) return 0x10;
    if(false > 0) return 0x10;
    
    int puzzleID1, puzzleID2;
    if(a8) { // assumes a8 is 1
        puzzleID1 = this->puzzle_ids_1[a3];
        puzzleID2 = this->puzzle_ids_1[a3+1];
    } else {
        puzzleID1 = this->puzzle_ids_2[a3];
        puzzleID2 = this->puzzle_ids_2[a3+1];
    }
    
    Puzzle *p1, *p2;
    p1 = this->doc->puzzles[puzzleID1];
    p2 = this->doc->puzzles[puzzleID2];
    
    this->AddRequiredQuestWithItemID(p1->item_1, distance);
    this->AddRequiredQuestWithItemID(p2->item_1, distance);
    
    if(!this->RequiredItemForZoneWasNotPlaced(zone_id)) {
        this->RemoveQuestRequiringItem(p1->item_1);
        // RemoveQuestRequiringItem(0);
        
        Message("WorldGenerator::Unknown_1 => 0\n");
        return 0;
    }
    
    if(this->ChooseItemIDFromZone_1(zone_id, puzzleID1, distance, p1->item_1, 0) >= 0) {
        this->Unknown_14(zone_id, puzzleID1, distance, p2->item_1);
    }
    
    this->addrequiredItemQuestsFromHotspots(zone_id);
    
    Message("WorldGenerator::Unknown_1 => %d\n", 1);
    return 1;
}


int WorldGenerator::Unknown_14(int16 zoneID, int16 a3, uint16 distance, uint16 providedItemID)
{
    Message("WorldGenerator::Unknown_14(%d, %d, %d, %d)\n", zoneID, a3, distance, providedItemID);
    if(zoneID < 0) return 0;
    
    Zone *zone = this->doc->zones[zoneID];
    
    for(uint16 itemID : zone->providedItemIDs)
        if(itemID == providedItemID) {
            vector<Hotspot*> hotspots;
            for(Hotspot* hotspot : zone->_hotspots) {
                if(hotspot->type == TriggerLocation) {
                    hotspots.push_back(hotspot);
                }
            }
            
            if ( hotspots.size() ) {
                this->AddRequiredQuestWithItemID(itemID, distance);
                this->wg_last_added_item_id = providedItemID;
                
                Hotspot *hotspot = hotspots[win_rand() % hotspots.size()];
                hotspot->arg1 = providedItemID;
                hotspot->enabled = 1;
                
                Message("WorldGenerator::Unknown_14 => %d\n", 1);
                return 1;
            }
            break;
        }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn)
        {
            int result = this->Unknown_14(hotspot->arg1, a3, distance, providedItemID);
            if(result) {
                Message("WorldGenerator::Unknown_14 => %d\n", result);
                return result;
            }
        }
    }
    
    Message("WorldGenerator::Unknown_14 => %d\n", 0);
    return 0;
}

#pragma mark - Bached
void WorldGenerator::WritePlanetValues() {
    /*
     switch (this->planet) {
     case TATOOINE: doc->WriteTatooineValues(); break;
     case HOTH: doc->WriteHothValues(); break;
     case ENDOR: doc->WriteEndorValues(); break;
     }
     */
}

signed int WorldGenerator::GenerateWorld(int seed, int puzzle_count, int16* map, int16 *puzzleMap) {
    return 0;
}

int WorldGenerator::PuzzleUsedInLastGame(uint16 puzzle_id, Planet planet) {
    return 0;
}

void WorldGenerator::GetTileProvidedByZonesHotspots(int16 zone_id)
{
    /*
     vector<Zone*> *tile_id; // eax@1
     Zone *zone; // edi@2
     int v4; // ebp@3
     int index; // esi@4
     Hotspot *hotspot; // ebx@5
     int x; // [sp-10h] [bp-20h]@9
     int y; // [sp-Ch] [bp-1Ch]@9
     int16 tile_id_1; // [sp-4h] [bp-14h]@9
     
     if ( zone_id >= 0 )
     {
     tile_id = &this->zones;
     zone = (*tile_id)[zone_id];
     if ( zone )
     {
     v4 = (int)zone->_hotspots.size();
     if ( v4 > 0 )
     {
     index = 0;
     do
     {
     hotspot = zone->_hotspots[index];
     tile_id = 0;
     switch ( hotspot->type )
     {
     case TriggerLocation:
     case SpawnLocation:
     case ForceLocation:
     case LocatorThingy:
     case CrateItem:
     case PuzzleNPC:
     case CrateWeapon:
     if ( hotspot->enabled == 1 && hotspot->arg1 >= 0 )
     {
     int id = zone->GetTileIdAt(zone, hotspot->x, hotspot->y, OBJECTS);
     if ( id < 0 )
     {
     tile_id_1 = hotspot->arg1;
     y = hotspot->y;
     x = hotspot->x;
     goto get_tile;
     }
     }
     break;
     case Unused:
     if ( hotspot->enabled == 1 )
     {
     hotspot->arg1 = ADEGAN_CRYSTAL;
     int tile_id = zone->GetTileIdAt(hotspot->x, hotspot->y, OBJECTS);
     if ( tile_id < 0 )
     {
     tile_id_1 = ADEGAN_CRYSTAL;
     y = hotspot->y;
     x = hotspot->x;
     get_tile:
     tile_id = zone->SetTileAt(zone, x, y, 1, tile_id_1);
     }
     }
     break;
     default:
     break;
     }
     ++index;
     --v4;
     }
     while ( v4 );
     }
     }
     }
     //        return (unsigned int)tile_id;
     */
}

#pragma mark - DONE -
int WorldGenerator::ContainsPuzzleId(const uint16 puzzle_id)
{
    for(uint16 id : this->puzzle_ids)
        if(id == puzzle_id) return 1;
    
    return 0;
}

bool WorldGenerator::ZoneHasItem(uint16 zoneID, uint16 targetItemID, int a4)
{
    Message("WorldGenerator::ZoneHasItem(%d, %d, %d)\n", zoneID, targetItemID, a4);
    Zone *zone = getZoneByID(zoneID);
    if ( !zone ) return 0;
    
    vector<uint16> &itemIDs = a4 ? zone->assignedItemIDs : zone->requiredItemIDs;
    for(uint16 itemID : itemIDs)
        if(itemID == targetItemID)
            return 1;
    
    for(Hotspot *hotspot : zone->_hotspots)
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0)
            if(ZoneHasItem(hotspot->arg1, targetItemID, a4))
                return 1;
    
    return 0;
}

Zone* WorldGenerator::getZoneByID(const uint16 zoneID) {
    return doc->zones[zoneID];
}

uint16 WorldGenerator::getZoneID(const Zone *zone){
    for(int i=0; i < doc->zones.size(); i++)
        if(doc->zones[i] == zone) return i;
    
    return -1;
}

int WorldGenerator::HasQuestRequiringItem(const uint16 itemID) {
    Message("WorldGenerator::HasQuestRequiringItem(%d)\n", itemID);
    for(Quest *quest : this->requiredItemQuests)
        if(quest->itemID == itemID) return 1;
    
    return 0;
}

Quest* WorldGenerator::AddProvidedQuestWithItemID(uint16 itemID, uint16 maximumDistance){
    Message("WorldGenerator::AddProvidedQuestWithItemID(%d, %d)\n", itemID, maximumDistance);
    
    for(Quest *quest : this->providedItemQuests)
        if(quest->itemID == itemID) return quest;
    
    Quest *quest = new Quest(itemID, maximumDistance);
    this->providedItemQuests.insert(this->providedItemQuests.begin(), quest);
    
    return quest;
}

Quest* WorldGenerator::AddRequiredQuestWithItemID(uint16 itemID, uint16 maximumDistance){
    Message("WorldGenerator::AddRequiredQuestWithItemID(%d, %d)\n", itemID, maximumDistance);
    
    Quest *quest = new Quest(itemID, maximumDistance);
    this->requiredItemQuests.push_back(quest);
    
    return quest;
}

void WorldGenerator::RemoveQuestProvidingItem(uint16 itemID) {
    Message("WorldGenerator::RemoveQuestProvidingItem(%d)\n", itemID);
    
    for(int i=0; i < this->providedItemQuests.size(); i++)
        if(this->providedItemQuests[i]->itemID == itemID) {
            this->providedItemQuests.erase(this->providedItemQuests.begin()+i);
            return;
        }
}

void WorldGenerator::RemoveQuestRequiringItem(uint16 itemID) {
    Message("WorldGenerator::RemoveQuestRequiringItem(%d)\n", itemID);
    
    for(int i=0; i < this->requiredItemQuests.size(); i++)
        if(this->requiredItemQuests[i]->itemID == itemID) {
            this->requiredItemQuests.erase(this->requiredItemQuests.begin()+i);
            return;
        }
}

signed int WorldGenerator::RequiredItemForZoneWasNotPlaced(const uint16 zone_id)
{
    Message("WorldGenerator::RequiredItemForZoneWasNotPlaced(%d)\n", zone_id);
    
    Zone *zone = doc->zones[zone_id];
    if ( !zone ) return 0;
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->arg1 <= 0) continue;
        
        switch (hotspot->type) {
            case DoorIn:
                if(!RequiredItemForZoneWasNotPlaced(hotspot->arg1))
                    return 0;
                break;
                
            case CrateItem:
            case PuzzleNPC:
            case CrateWeapon:
                if(HasQuestRequiringItem(hotspot->arg1))
                    return 0;
                break;
                
            default: break;
        }
    }
    
    return 1;
}


signed int WorldGenerator::ChooseItemIDFromZone(uint16 zoneID, uint16 itemID, int fromAssignedItems)
{
    Message("WorldGenerator::ChooseItemIDFromZone(%d, %d, %d)\n", zoneID, itemID, fromAssignedItems);
    Zone *zone = getZoneByID(zoneID);
    if (!zone) return -1;
    
    vector<uint16> &itemIDs = fromAssignedItems ? zone->assignedItemIDs : zone->requiredItemIDs;
    for(uint16 candidate : itemIDs) {
        if(candidate == itemID) {
            return candidate;
        }
    }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0) {
            int result = ChooseItemIDFromZone(hotspot->arg1, itemID, fromAssignedItems);
            if(result >= 0) return result;
        }
    }
    
    return -1;
}

uint16 WorldGenerator::getZoneIDAt(const int x,const  int y){
    return this->worldThings[x + 10 * y].zone_id;
}

int WorldGenerator::getLocationOfZoneWithID(uint16 zoneID, int *xOut, int *yOut) {
    for(int y=0; y < 10; y++)
        for(int x=0; x < 10; x++)
            if(this->worldThings[x + 10 * y].zone_id == zoneID) {
                *xOut = x;
                *yOut = y;
                return 1;
            }
    
    return 0;
}

int WorldGenerator::worldContainsZoneId(uint16 zoneID) {
    Message("WorldGenerator::WorldContainsZoneId(%d)\n", zoneID);
    for(uint16 chosenZoneID : this->chosen_zone_ids)
        if(chosenZoneID == zoneID) {
            Message("WorldGenerator::WorldContainsZoneId => 1\n");
            return 1;
        }
    Message("WorldGenerator::WorldContainsZoneId => 0\n");
    return 0;
}

void WorldGenerator::addZoneWithIdToWorld(const uint16 zoneID){
    Message("WorldGenerator::addZoneWithIdToWorld(%d)\n", zoneID);
    if(zoneID >= doc->zones.size()) {
        printf("Invalid Zone ID\n");
        return;
    }
    
    this->chosen_zone_ids.insert(this->chosen_zone_ids.begin(), zoneID);
}

void WorldGenerator::addrequiredItemQuestsFromHotspots(uint16 zoneID) {
    Message("WorldGenerator::addrequiredItemQuestsFromHotspots(%d)\n", zoneID);
    Zone* zone = doc->zones[zoneID];
    for (Hotspot* hotspot : zone->_hotspots) {
        switch (hotspot->type) {
            case CrateItem:
            case PuzzleNPC:
            case CrateWeapon:
                if(hotspot->arg1 != -1)
                    this->AddRequiredQuestWithItemID(hotspot->arg1, -1);
                break;
            case DoorIn:
                if(hotspot->arg1 != -1)
                    this->addrequiredItemQuestsFromHotspots(hotspot->arg1);
                break;
            default:
                break;
        }
    }
}

int16 WorldGenerator::findUnusedNPCForZone(uint16 zone_id)
{
    Message("WorldGenerator::findUnusedNPCForZone(%d)\n", zone_id);
    Zone *zone = getZoneByID(zone_id);
    if (!zone) return -1;
    
    vector<uint16> npcCandidates;
    for(int16 npcTileID : zone->puzzleNPCTileIDs)
        if(!HasQuestRequiringItem(npcTileID))
            npcCandidates.push_back(npcTileID);
    
    if(npcCandidates.size()) {
        int idx = win_rand() % npcCandidates.size();
        return npcCandidates[idx];
    }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0) {
            int result = findUnusedNPCForZone(hotspot->arg1);
            if(result >= 0) return result;
        }
    }
    
    return -1;
}

bool WorldGenerator::hasPuzzleNPC(uint16 zoneID, int16 targetNPCID)
{
    Message("WorldGenerator::hasPuzzleNPC(%d, %d)\n", zoneID, targetNPCID);
    
    Zone *zone = getZoneByID(zoneID);
    if (!zone ) return 0;
    
    if ( targetNPCID == -1 ) {
        printf("Is this really a valid call?\n");
        return zone->puzzleNPCTileIDs.size() != 0;
    }
    
    for(uint16 npcID : zone->puzzleNPCTileIDs)
        if(npcID == targetNPCID) return true;
    
    for(Hotspot *hotspot : zone->_hotspots)
        if (hotspot->type == DoorIn && hotspot->arg1 != -1)
            if(hasPuzzleNPC(hotspot->arg1, targetNPCID))
                return true;
    
    return false;
}

bool WorldGenerator::ZoneHasProvidedItem(uint16 zoneID, uint16 itemID) {
    Message("WorldGenerator::ZoneHasProvidedItem(%d, %d)\n", zoneID, itemID);
    Zone *zone = getZoneByID(zoneID);
    for(uint16 itemIDInZone : zone->providedItemIDs)
        if(itemIDInZone == itemID) return true;
    
    for(Hotspot* hotspot : zone->_hotspots)
        if(hotspot->type == DoorIn && hotspot->arg1 != -1 && ZoneHasProvidedItem(hotspot->arg1, itemID))
            return true;
    
    return false;
}


signed int WorldGenerator::ChooseSpawnForPuzzleNPC(int16 zoneID, int npcID)
{
    Message("WorldGenerator::ChooseSpawnForPuzzleNPC(%d, %d)\n", zoneID, npcID);
    Zone *zone = getZoneByID(zoneID);
    if ( !zone ) return -1;
    
    vector<Hotspot*> hotspotCandidates;
    for(uint16 npcTileID : zone->puzzleNPCTileIDs) {
        if(npcTileID != npcID) continue;
        
        hotspotCandidates.clear();
        for(Hotspot *hotspot : zone->_hotspots) {
            if(hotspot->type == SpawnLocation && hotspot->arg1 == -1) {
                hotspotCandidates.push_back(hotspot);
            }
        }
        
        if(hotspotCandidates.size()) {
            int idx = win_rand() % hotspotCandidates.size();
            Hotspot *hotspot = hotspotCandidates[idx];
            
            hotspot->arg1 = npcID;
            hotspot->enabled = 1;
            this->wg_npc_id = npcID;
            
            return npcID;
        }
    }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0) {
            int result = ChooseSpawnForPuzzleNPC(hotspot->arg1, npcID);
            if(result != -1) return result;
        }
    }
    
    return -1;
}


int WorldGenerator::PuzzleIsGoal(uint16 puzzle_id, Planet planet){
    if ( planet == TATOOINE ) {
        switch ( puzzle_id ) {
            case GOAL_FALCON:
            case GOAL_HAN:
            case GOAL_AMULET:
            case GOAL_ADEGAN_CRYSTAL:
            case GOAL_THREEPIOS_PARTS:
                return 1;
            default:
                return 0;
        }
    }
    
    if ( planet == HOTH ) {
        switch ( puzzle_id ) {
            case GOAL_GENERAL_MARUTZ:
            case GOAL_HIDDEN_FACTORY:
            case GOAL_WARN_THE_REBELS:
            case GOAL_RESCUE_YODA:
            case GOAL_CAR:
                return 1;
            default:
                return 0;
        }
    }
    
    if ( planet == ENDOR ) {
        switch ( puzzle_id ) {
            case GOAL_FIND_LEIA:
            case GOAL_IMPERIAL_BATTLE_STATION:
            case GOAL_LANTERN_OF_SACRED_LIGHT:
            case GOAL_IMPERIAL_BATTLE_CODE:
            case GOAL_RELAY_STATION:
                return 1;
            default:
                return 0;
        }
    }
    
    return 0;
}

int WorldGenerator::GetItemIDThatsNotRequiredYet(uint16 zoneID, int unused, bool use_array_2_ids)
{
    Message("WorldGenerator::GetItemIDThatsNotRequiredYet(%d, %d, %d)\n", zoneID, unused, use_array_2_ids);
    vector<uint16> itemIDs;
    Zone *zone = doc->zones[zoneID];
    
    vector<uint16> &zoneItemIds = use_array_2_ids ? zone->assignedItemIDs : zone->requiredItemIDs;
    for(uint16 itemID : zoneItemIds)
        if(!HasQuestRequiringItem(itemID))
            itemIDs.push_back(itemID);
    
    if(itemIDs.size())
        return itemIDs[win_rand() % itemIDs.size()];
    
    for(Hotspot *hotspot : zone->_hotspots){
        if(hotspot->type == DoorIn && hotspot->arg1 != -1) {
            int16 itemID = GetItemIDThatsNotRequiredYet(hotspot->arg1, unused, use_array_2_ids);
            if(itemID != -1) return itemID;
        }
    }
    
    return -1;
}

bool WorldGenerator::findPlaceToPutPuzzle(int maxDistance, int16 *world, int* xref, int* yref) {
    Message("WorldGenerator::findPlaceToPutPuzzle(%d, .., .., ..)\n", maxDistance);
    vector<MapPoint*> farPoints;
    vector<MapPoint*> bestPoints;
    vector<MapPoint*> pointsCloseToPuzzles;
    
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            int idx = x + 10 * y;
            int item = world[idx];
            MapPoint *point = new MapPoint(x, y);
            if ( MapGenerator::GetDistanceToCenter(x, y) > maxDistance )  {
                if ( item == 1 ) {
                    Message("1) %dx%d\n", x, y);
                    farPoints.push_back(point);
                } else delete point;
            } else if ( item == 1 || item == 300 ) {
                Message("2) %dx%d\n", x, y);
                if (( x < 1 || world[idx-1] != 306 )
                    && ( x > 8 || world[idx+1] != 306 )
                    && ( y < 1 || world[idx-10] != 306 )
                    && ( y > 8 || world[idx+10] != 306 ))
                    bestPoints.push_back(point);
                else
                    pointsCloseToPuzzles.push_back(point);
            } else delete point;
        }
    }
    
    int idx = 0;
    vector<MapPoint*>*array = NULL;
    if(bestPoints.size()){
        idx = win_rand() % bestPoints.size();
        array = &bestPoints;
    } else if(pointsCloseToPuzzles.size()) {
        idx = win_rand() % pointsCloseToPuzzles.size();
        array = &pointsCloseToPuzzles;
    } else if(farPoints.size()){
        idx = win_rand() % farPoints.size();
        array = &farPoints;
    } else {
        Message("No Place to put puzzle!\n");
        return false;
    }
    
    if(array) {
        MapPoint *chosenPoint = (*array)[idx];
        *xref = chosenPoint->x;
        *yref = chosenPoint->y;
        Message("WorldGenerator::findPlaceToPutPuzzle: %dx%d\n", chosenPoint->x, chosenPoint->y);
    }
    
    for(MapPoint *point : farPoints) {
        delete point;
    }
    farPoints.clear();
    for(MapPoint *point : bestPoints) {
        delete point;
    }
    bestPoints.clear();
    for(MapPoint *point : pointsCloseToPuzzles) {
        delete point;
    }
    pointsCloseToPuzzles.clear();
    
    return true;
}

#pragma mark - TODO -
void WorldGenerator::ShuffleVector(vector<int16> &array) {
    size_t count = array.size();
    if ( count == 0 ) return;
    
    vector<int16> temp_array(count, -1);
    
    for(int i=0; i < count; i++) {
        int idx = win_rand() % count;
        if ( temp_array[idx] == -1 ) {
            temp_array[idx] = array[i];
            array[i] = -1;
        }
    }
    
    for(size_t i = count-1; i != 0; i--) {
        int did_find_free_spot = 0;
        while(true) {
            for(int i=0; i < count; i++)
                if ( temp_array[i] == -1 )
                    did_find_free_spot = 1;
            
            if ( !did_find_free_spot ) break;
            
            int idx = win_rand() % count;
            if ( temp_array[idx] == -1 ) {
                temp_array[idx] = array[i];
                array[i] = -1;
                break;
            }
        }
    }
    
    for(int i=0; i < count; i++) {
        array[i] = temp_array[i];
    }
}

void WorldGenerator::GetPuzzleCandidates(vector<int16> &result, uint16 item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    result.clear();
    
    for(int16 puzzle_idx = 0; puzzle_idx < doc->puzzles.size(); puzzle_idx++) {
        Puzzle * puzzle = doc->puzzles[puzzle_idx];
        if ( (int16)zone_type <= (signed int)ZONETYPE_Trade ) {
            if ( zone_type == ZONETYPE_Trade ) {
                if ( puzzle->type != PuzzleTypeTrade || this->ContainsPuzzleId(puzzle_idx) )
                    continue;
            } else if ( zone_type != ZONETYPE_Goal
                       || puzzle->type != PuzzleTypeU3
                       || this->ContainsPuzzleId(puzzle_idx) ) {
                continue;
            }
            
            result.insert(result.end(), puzzle_idx);
            continue;
        }
        
        if ( zone_type == ZONETYPE_Use )
        {
            if (!puzzle->type && !this->ContainsPuzzleId(puzzle_idx) )
                result.insert(result.end(), puzzle_idx);
            
            continue;
        }
        
        if ( zone_type != 9999 || puzzle->type != PuzzleTypeEnd )
            continue;
        
        if ( this->PuzzleUsedInLastGame(puzzle_idx, doc->planet) && doc->puzzles_can_be_reused < 0 )
            continue;
        
        if(this->PuzzleIsGoal(puzzle_idx, doc->planet)) {
            result.insert(result.end(), puzzle_idx);
        }
    }
}


signed int WorldGenerator::ChooseItemIDFromZone_0(int16 zone_id, int itemID)
{
    Message("WorldGenerator::ChooseItemIDFromZone_0(%d, %d)\n", zone_id, itemID);
    vector<Hotspot*> hotspotCandidates;
    Zone *zone = getZoneByID(zone_id);
    if(!zone) return -1;
    
    Message("v16 = %d\n", 0);
    for(int providedItemID : zone->providedItemIDs) {
        if ( itemID == providedItemID ) {
            hotspotCandidates.clear();
            
            for(Hotspot *hotspot : zone->_hotspots)
                if(hotspot->type == TriggerLocation)
                    hotspotCandidates.insert(hotspotCandidates.begin(), hotspot);
            
            if ( hotspotCandidates.size() > 0 ) {
                int idx = win_rand() % hotspotCandidates.size();
                Hotspot *hotspot = hotspotCandidates[idx];
                if (hotspot->type == TriggerLocation ) {
                    hotspot->arg1 = providedItemID;
                    hotspot->enabled = 1;
                    return providedItemID;
                }
            }
        }
    }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0) {
            int result = this->ChooseItemIDFromZone_0(hotspot->arg1, itemID);
            if(result >= 0) return result;
        }
    }
    
    return -1;
}

signed int WorldGenerator::ChooseItemIDFromZone_1(int16 zoneID, int a3, int a4, int16 item_id, int a6)
{
    Message("WorldGenerator::ChooseItemIDFromZone_1(%d, %d, %d, %d, %d)\n", zoneID, a3, a4, item_id, a6);
    
    if ( zoneID < 0 ) {
        Message("WorldGenerator::ChooseItemIDFromZone_1() => %d\n", 0);
        return 0;
    }
    
    Zone *zone = doc->zones[zoneID];
    vector<uint16> &itemIDs = a6 ? zone->assignedItemIDs : zone->requiredItemIDs;
    for(uint16 itemID : itemIDs) {
        if(itemID != item_id) continue;
        
        for(Hotspot *hotspot : zone->_hotspots) {
            if(hotspot->type == Lock) {
                AddRequiredQuestWithItemID(item_id, a4);
                
                if ( a6 ) this->wg_item_id_unknown_3 = item_id;
                else this->wg_item_id = item_id;
                
                hotspot->arg1 = item_id;
                hotspot->enabled = 1;
                
                Message("WorldGenerator::ChooseItemIDFromZone_1() => %d\n", 1);
                return 1;
            }
        }
    }
    
    for(Hotspot* hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn) {
            int result = ChooseItemIDFromZone_1(hotspot->arg1, a3, a4, item_id, a6);
            if(result) return result;
        }
    }
    
    Message("WorldGenerator::ChooseItemIDFromZone_1() => %d\n", 0);
    return 0;
}

signed int WorldGenerator::use_ids_from_array_1(int16 zone_id, int16 a3, int16 item_id_1, int16 a5)
{
    Message("WorldGenerator::use_ids_from_array_1(%d, %d, %d, %d)\n", zone_id, a3, item_id_1, a5);
    Zone *zone = NULL; // eax@3
    vector<uint16> *v9 = NULL; // eax@14
    int16 v10 = 0; // dx@17
    vector<Puzzle*>* v11 = NULL; // ebx@19
    Puzzle *v12 = NULL; // ecx@19
    Puzzle *v13 = NULL; // eax@22
    int16 item_id = 0; // bp@25
    int16 a3a = 0; // [sp+12h] [bp-Ah]@25
    Puzzle *a3_2 = NULL; // [sp+14h] [bp-8h]@0
    
    if ( zone_id < 0 ) return 0;
    zone = doc->zones[zone_id];
    
    if ( !zone ) return 0;
    
    if (zone->getType() != ZONETYPE_Use) return 0;
    if (!this->RequiredItemForZoneWasNotPlaced(zone_id)) return 0;
    
    if ( a5 ) {
        if ( this->puzzle_ids_1.size() - 1 > a3 ) {
            v9 = &this->puzzle_ids_1;
            v10 = (*v9)[a3 + 1];
        } else { v10 = -1; }
    } else if ( this->puzzle_ids_2.size() - 1 > a3 ) {
        v9 = &this->puzzle_ids_2;
        v10 = (*v9)[a3 + 1];
    } else v10 = -1;
    
    int16 v8 = a5 ? this->puzzle_ids_1[a3] : this->puzzle_ids_2[a3];
    
    v11 = &doc->puzzles;
    v12 = (*v11)[v8];
    if ( !v12 )  return 0;
    if ( v10 < 0 ) {
        v13 = a3_2;
    } else {
        v13 = (*v11)[v10];
        if ( !v13 )  return 0;
    }
    item_id = -1;
    a3a = v12->item_1;
    if ( v10 >= 0 ) item_id = v13->item_1;
    int16 npcID = this->findUnusedNPCForZone(zone_id);
    if ( npcID < 0 ) return 0;
    
    int v19 = 1;
    int a3_2a = this->ZoneHasItem(zone_id, a3a, 0);
    if ( item_id >= 0 )
        v19 = this->ZoneHasProvidedItem(zone_id, item_id);
    
    if ( !a3_2a || !v19 ) return 0;
    
    if ( this->ChooseSpawnForPuzzleNPC(zone_id, npcID) < 0 ) return 0;
    
    this->wg_npc_id = npcID;
    this->wg_last_added_item_id = item_id;
    this->wg_item_id = a3a;
    this->field_3394 = a3;
    
    this->AddRequiredQuestWithItemID(npcID, item_id_1);
    this->addrequiredItemQuestsFromHotspots(zone_id);
    
    return 1;
}

int WorldGenerator::Unknown_7(int16 zone_id, int16 puzzle_idx, int16 a4, int unknown, int a6)
{
    Message("WorldGenerator::Unknown_7(%d, %d, %d, %d, %d)\n", zone_id, puzzle_idx, a4, unknown, a6);
    Zone *zone = NULL;
    signed int result = 0;
    uint16 item = 0;
    signed int v14 = 0;
    int v15 = 0;
    signed int v16 = 0;
    signed int v17 = 0;
    signed int v19 = 0;
    signed int v21 = 0;
    signed int v22 = 0;
    signed int v23 = 0;
    signed int v24 = 0;
    
    zone = doc->zones[zone_id];
    if (!zone) return 0;
    
    if ( zone->getType() != ZONETYPE_Goal ) return 0;
    
    if (!this->RequiredItemForZoneWasNotPlaced(zone_id) )
        return 0;
    
    Puzzle *puzzle1 = doc->puzzles[this->puzzle_ids_1[puzzle_idx]];
    Puzzle *puzzle3 = doc->puzzles[this->puzzle_ids_1[puzzle_idx+1]];
    Puzzle *puzzle2 = doc->puzzles[this->puzzle_ids_2[a4]];
    
    puzzle1->unknown_3 = 0;
    v14 = this->findUnusedNPCForZone(zone_id);
    v15 = v14;
    if ( v14 >= 0 )
        puzzle1->unknown_3 = hasPuzzleNPC(zone_id, v14);
    
    v16 = this->ZoneHasItem(zone_id, puzzle1->item_1, 0);
    this->ZoneHasItem(zone_id, puzzle2->item_1, 1);
    v23 = this->ZoneHasProvidedItem(zone_id, puzzle3->item_1);
    this->ZoneHasProvidedItem(zone_id, puzzle3->item_2);
    if ( !v16 || !v23 )
        return 0;
    
    if ( puzzle1->unknown_3 ) {
        this->addrequiredItemQuestsFromHotspots(zone_id);
        
        this->wg_npc_id = v15;
        this->wg_last_added_item_id = item;
        this->wg_item_id_unknown_2 = puzzle1->item_1;
        this->wg_item_id = puzzle1->item_1;
        this->field_3394 = puzzle_idx;
        this->field_3398 = a4;
        this->wg_item_id_unknown_3 = puzzle1->item_1;
        this->addrequiredItemQuestsFromHotspots(zone_id);
        result = 1;
    } else {
        v17 = this->ChooseItemIDFromZone(zone_id, puzzle1->item_1, 0);
        v22 = this->ChooseItemIDFromZone_0(zone_id, puzzle3->item_1);
        if ( v17 >= 0 && v22 >= 0 ) {
            this->AddRequiredQuestWithItemID(puzzle1->item_1, unknown);
            this->AddRequiredQuestWithItemID(puzzle3->item_1, unknown);
        }
        v24 = this->ChooseItemIDFromZone(zone_id, puzzle2->item_1, 1);
        v19 = this->ChooseItemIDFromZone_0(zone_id, puzzle3->item_2);
        v21 = v19;
        if ( v24 >= 0 && v19 >= 0 ) {
            this->AddRequiredQuestWithItemID(puzzle2->item_1, unknown);
            this->AddRequiredQuestWithItemID(puzzle3->item_2, unknown);
        }
        if ( v17 < 0 || v22 < 0 || v24 < 0 || v21 < 0 ) {
            this->RemoveQuestRequiringItem(puzzle1->item_1);
            this->RemoveQuestRequiringItem(item);
            this->RemoveQuestRequiringItem(puzzle1->item_1);
            this->RemoveQuestRequiringItem(puzzle1->item_1);
            result = 0;
        } else {
            this->wg_npc_id = -1;
            this->wg_last_added_item_id = puzzle3->item_1;
            this->wg_item_id = puzzle1->item_1;
            this->field_3394 = puzzle_idx;
            this->wg_item_id_unknown_2 = puzzle1->item_1;
            this->wg_item_id_unknown_3 = puzzle1->item_1;
            this->field_3398 = a4;
            addrequiredItemQuestsFromHotspots(zone_id);
            result = 1;
        }
    }
    
    return result;
}

signed int WorldGenerator::SetupRequiredItemForZone_(int16 zone_id, int16 arg2, int use_required_items_array)
{
    Message("WorldGenerator::SetupRequiredItemForZone_(%d, %d, %d)\n", zone_id, arg2, use_required_items_array);
    int count; // esi@6
    int idx; // edi@12
    uint16 item_id; // bx@13
    int idx_1; // edi@18
    uint16 v10; // bx@19
    int count_2; // esi@23
    int random_item_id; // esi@23
    signed int v14; // ebx@24
    int count_1; // ecx@24
    int16 zone_id_1; // di@32
    vector<uint16> item_ids_1; // [sp+Ch] [bp-28h]@1
    Zone *zone; // [sp+20h] [bp-14h]@4
    
    item_ids_1.clear();
    
    if ( zone_id < 0 ){
        return 0;
    }
    
    zone = doc->zones[zone_id];
    if ( !zone ) {
        return 0;
    }
    
    count = use_required_items_array ? (int)zone->assignedItemIDs.size() : (int)zone->requiredItemIDs.size();
    if ( !count || !RequiredItemForZoneWasNotPlaced(zone_id)) {
        return 0;
    }
    
    if ( use_required_items_array ) {
        if ( count > 0 ) {
            idx = 0;
            do {
                item_id = zone->assignedItemIDs[idx];
                if ( !HasQuestRequiringItem(zone->assignedItemIDs[idx]) ) {
                    item_ids_1.push_back(item_id);
                }
                ++idx;
                --count;
            }
            while ( count );
        }
    }
    else if ( count > 0 ) {
        idx_1 = 0;
        do {
            v10 = zone->requiredItemIDs[idx_1];
            if ( !HasQuestRequiringItem(zone->requiredItemIDs[idx_1]) )
                item_ids_1.push_back(v10);
            ++idx_1;
            --count;
        }
        while ( count );
    }
    
    if ( !item_ids_1.size() ) {
        return 0;
    }
    
    count_2 = (int)item_ids_1.size();
    random_item_id = item_ids_1[win_rand() % count_2];
    if ( zone->providedItemIDs.size() == 1 ) {
        int idx = 0;
        v14 = 0;
        count_1 = (int)this->item_ids.size();
        if ( count_1 > 0 ) {
            int documentItemIdx = 0;
            do {
                if ( this->item_ids[documentItemIdx] == zone->providedItemIDs[idx])
                    v14 = 1;
                ++documentItemIdx;
                --count_1;
            }
            while ( count_1 );
        }
        if ( !v14 ) {
            this->item_ids.push_back(zone->providedItemIDs[idx]);
        } else return 0;
    }
    
    if ( zone->getType() == ZONETYPE_TravelStart ) {
        AddProvidedQuestWithItemID(random_item_id, 5);
        zone_id_1 = arg2;
    } else {
        zone_id_1 = arg2;
        AddProvidedQuestWithItemID(random_item_id, arg2);
    }
    AddRequiredQuestWithItemID(random_item_id, zone_id_1);
    
    this->wg_item_id = random_item_id;
    addrequiredItemQuestsFromHotspots(zone_id);
    
    return 1;
}

int WorldGenerator::AssociateItemWithZoneHotspot(int16 zone_id, int item_id, int a4)
{
    Message("WorldGenerator::AssociateItemWithZoneHotspot(%d, %d, %d)\n", zone_id, item_id, a4);
    signed int found_item_id_in_zone_items; // esi@1
    Zone *zone; // edi@3
    int hotspot_type; // ebx@12
    int tile_specs; // eax@13
    int idx; // esi@19
    Hotspot *hotspot; // eax@24
    int idx_1; // esi@26
    int v16; // ebx@26
    Hotspot *hotspot_1; // eax@27
    int zone_id_1; // edx@28
    vector<uint16> v20; // [sp+Ch] [bp-30h]@13
    int v24; // [sp+24h] [bp-18h]@19
    int didFindSuitableHotspot; // [sp+28h] [bp-14h]@1
    
    found_item_id_in_zone_items = 0;
    didFindSuitableHotspot = 0;
    if ( zone_id >= 0 ) {
        if ( RequiredItemForZoneWasNotPlaced(zone_id) ) {
            zone = doc->zones[zone_id];
            if ( zone ) {
                if ( zone->requiredItemIDs.size() <= 0 && zone->puzzleNPCTileIDs.size() <= 0 ) {
                    for(uint16 itemID : zone->providedItemIDs) {
                        if(itemID == item_id) {
                            found_item_id_in_zone_items = 1;
                            break;
                        }
                    }
                    hotspot_type = 0;
                    if ( found_item_id_in_zone_items ) {
                        v20.clear();
                        v20.resize(0);
                        tile_specs = doc->tiles[item_id]->_specs;
                        if ( tile_specs & TILE_SPEC_THE_FORCE ) {
                            hotspot_type = ForceLocation;
                        } else if ( tile_specs & TILE_SPEC_MAP ) {
                            hotspot_type = LocatorThingy;
                        } else if ( (uint8_t)tile_specs & (uint8_t)TILE_SPEC_USEFUL ) {
                            hotspot_type = TriggerLocation;
                        }
                        idx = 0;
                        v24 = (int)zone->_hotspots.size();
                        if ( v24 > 0 ) {
                            do {
                                if ( zone->_hotspots[idx]->type == hotspot_type ) {
                                    v20.push_back(idx);
                                    didFindSuitableHotspot = 1;
                                }
                                ++idx;
                            }
                            while ( v24 > idx );
                        }
                        
                        if ( didFindSuitableHotspot == 1 ) {
                            hotspot = zone->_hotspots[v20[win_rand() % v20.size()]];
                            hotspot->arg1 = item_id;
                            hotspot->enabled = 1;
                            this->AddRequiredQuestWithItemID(item_id, a4);
                            this->wg_last_added_item_id = item_id;
                        }
                    } else {
                        idx_1 = 0;
                        v16 = 0;
                        v24 = (int)zone->_hotspots.size();
                        if ( v24 > 0 ) {
                            do
                            {
                                hotspot_1 = zone->_hotspots[idx_1];
                                if ( hotspot_1->type == DoorIn )
                                {
                                    zone_id_1 = hotspot_1->arg1;
                                    if ( zone_id_1 >= 0 )
                                        didFindSuitableHotspot = this->AssociateItemWithZoneHotspot(zone_id_1, item_id, a4);
                                    if ( didFindSuitableHotspot == 1 ) {
                                        this->addrequiredItemQuestsFromHotspots(zone_id);
                                        return didFindSuitableHotspot;
                                    }
                                }
                                ++idx_1;
                            }
                            while ( v24 > ++v16 );
                        }
                    }
                    if ( didFindSuitableHotspot != 1 )
                        return didFindSuitableHotspot;
                    
                    this->addrequiredItemQuestsFromHotspots(zone_id);
                    return didFindSuitableHotspot;
                }
            }
        }
    }
    return 0;
}
