//
//  YodaDocument.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "YodaDocument.hpp"
#include "Map.hpp"

#define PRINT_ARRAY_SHUFFLE_RANDS 0

YodaDocument::YodaDocument(){
    puzzles_can_be_reused = -1;
    world_things.resize(100);
    for(int i=0; i < 100; i++) {
        world_things[i].zone_type = (ZONE_TYPE)-1;
        world_things[i].zone_id = -1;
    }
}

#define THE_FORCE 0x1ff
#define LOCATOR 0x1a5

int TILE_SPEC_THE_FORCE  = 0x40;
int TILE_SPEC_USEFUL  = 0x80;
int TILE_SPEC_MAP    = 0x100000;

void YodaDocument::ShuffleVector(vector<int16> &array) {
    vector<int16> temp_array;
    
    /*
     printf("Array::Shuffle (%lu items): ", array.size());
     for(int16 item : array) printf("%d, ", item);
     printf("\n");
    //*/
    int16 count = array.size();
    temp_array.resize(count, -1);
    if ( count <= 0 ) return;
    
    for(int i=0; i < count; i++) {
        int random = win_rand();
        if(PRINT_ARRAY_SHUFFLE_RANDS) printf("Array::Shuffle rand 1: %x\n", random);
        int idx = random % count;
        if ( temp_array[idx] == -1 ) {
            temp_array[idx] = array[i];
            array[i] = -1;
        }
    }
    
    int16 v18 = count - 1;
    if ( v18 >= 0 ) {
        do {
            // if (array[v18] != -1 || true) {
            int did_find_free_spot = 0;
            int16 v17 = 0;
            do {
                for(int i=0; i < count; i++) {
                    if ( temp_array[i] == -1 )
                        did_find_free_spot = 1;
                }
                
                if ( !did_find_free_spot ) break;
                
                int random = win_rand();
                if(PRINT_ARRAY_SHUFFLE_RANDS) printf("Array::Shuffle rand 2: %x\n", random);
                int idx = random % count;
                
                if ( temp_array[idx] == -1 ) {
                    ++v17;
                    temp_array[idx] = array[v18];
                    array[v18] = -1;
                }
            }
            while ( !v17 );
            // }
            --v18;
        }
        while ( v18 >= 0 );
    }
    
    for(int i=0; i < count; i++) {
        array[i] = temp_array[i];
    }
    
    /*
     printf("=> (%lu items): ", array.size());
     for(int16 item : array) printf("%d, ", item);
     printf("\n");
    //*/
}

int16 YodaDocument::GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    printf("YodaDocument::GetNewPuzzleId(%d, %d, %d, %d)\n", item_id, a3, zone_type, a5);
    int16 puzzle_id; // dx@57
    Puzzle *puzzle_1; // ebx@57
    int break_from_loop = 0;
    
    vector<int16> puzzle_ids;
    GetPuzzleCandidates(puzzle_ids, item_id, a3, zone_type, a5);
    if (puzzle_ids.size() <= 0 ) {
        return -1;
    }
    
    ShuffleVector(puzzle_ids);
    
    size_t count = puzzle_ids.size();
    int16 puzzle_idx = 0;
    while ( 1 )
    {
        puzzle_id = puzzle_ids[puzzle_idx];
        puzzle_1 = this->puzzles[puzzle_id];
        if ( ContainsPuzzleId(puzzle_id) )
            goto LABEL_75;
        if ( zone_type <= ZONETYPE_Trade )
        {
            if ( zone_type == ZONETYPE_Trade )
            {
                if ( puzzle_1->type == PuzzleTypeTrade && puzzle_1->questItemIDs[0] == item_id )
                {
                    printf("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
                    return puzzle_ids[puzzle_idx];
                }
            }
            else if ( zone_type == ZONETYPE_Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->questItemIDs[0] == item_id )
            {
                printf("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
                return puzzle_ids[puzzle_idx];
            }
            goto LABEL_75;
        }
        if ( zone_type != ZONETYPE_Use )
            break;
        if ( puzzle_1->type == PuzzleTypeU1 && puzzle_1->questItemIDs[0] == item_id )
        {
            printf("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
            return puzzle_ids[puzzle_idx];
        }
    LABEL_75:
        if ( !break_from_loop )
        {
            ++puzzle_idx;
            if ( puzzle_idx >= count )
                ++break_from_loop;
            if ( !break_from_loop )
                continue;
        }
        
        printf("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", -1, -1);
        return -1;
    }
    
    if ( zone_type != 9999 || puzzle_1->type != PuzzleTypeEnd )
        goto LABEL_75;
    
    printf("YodaDocument::GetNewPuzzleId => 0x%x (%d)\n", puzzle_ids[puzzle_idx], puzzle_ids[puzzle_idx]);
    return puzzle_ids[puzzle_idx];
}

void YodaDocument::GetPuzzleCandidates(vector<int16> &result, uint16 item_id, int a3, ZONE_TYPE zone_type, int a5) {
    Puzzle *puzzle; // edx@2
    __int16 count_3; // [sp+2Ah] [bp-16h]@1
    int16 puzzle_idx; // [sp+2Ch] [bp-14h]@1
    
    count_3 = this->puzzles.size();
    result.clear();
    
    puzzle_idx = 0;
    if ( count_3 > 0 )
    {
        while ( 1 )
        {
            puzzle = this->puzzles[puzzle_idx];
            if ( (int16)zone_type <= (signed int)ZONETYPE_Trade )
                break;
            if ( zone_type == ZONETYPE_Use )
            {
                if ( puzzle->type || ContainsPuzzleId(puzzle_idx) )
                    goto do_break;
            use_puzzle:
                result.insert(result.end(), puzzle_idx);
                goto do_break;
            }
            if ( zone_type != 9999 || puzzle->type != PuzzleTypeEnd )
                goto do_break;
            
            if ( PuzzleUsedInLastGame(puzzle_idx, this->planet) && this->puzzles_can_be_reused < 0 )
                goto do_break;
            
            if(PuzzleIsGoal(puzzle_idx, this->planet))
                goto use_puzzle;
            goto do_break;
            
        do_break:
            if ( count_3 <= (int16)++puzzle_idx )
                goto out_of_loop;
        }
        if ( zone_type == ZONETYPE_Trade )
        {
            if ( puzzle->type != PuzzleTypeTrade || ContainsPuzzleId(puzzle_idx) )
                goto do_break;
        }
        else if ( zone_type != ZONETYPE_Goal
                 || puzzle->type != PuzzleTypeU3
                 || ContainsPuzzleId(puzzle_idx) )
        {
            goto do_break;
        }
        goto use_puzzle;
    }
out_of_loop:
    return;
}

int YodaDocument::ContainsPuzzleId(uint16 puzzle_id)
{
    for(uint16 id : puzzle_ids) {
        if(id == puzzle_id) return 1;
    }
    return 0;
}

int YodaDocument::PuzzleUsedInLastGame(uint16 puzzle_id, Planet planet) {
    return 0;
}

int YodaDocument::PuzzleIsGoal(uint16 puzzle_id, Planet planet){
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
    
    if ( planet == ENDOR
        && puzzle_id >= (signed int)GOAL_FIND_LEIA
        && (puzzle_id <= (signed int)GOAL_IMPERIAL_BATTLE_STATION
            || puzzle_id == GOAL_LANTERN_OF_SACRED_LIGHT) ) {
            return 1;
        }
    
    return 0;
}
#pragma mark -
Zone* YodaDocument::getZoneByID(uint16 zoneID) {
    return zones[zoneID];
}

int YodaDocument::worldContainsZoneId(uint16 zoneID) {
    printf("YodaDocument::WorldContainsZoneId(%d)\n", zoneID);
    for(uint16 chosenZoneID : chosen_zone_ids)
        if(chosenZoneID == zoneID) return 1;
    
    return 0;
}
void YodaDocument::AddZoneWithIdToWorld(uint16 zoneID){
    printf("YodaDocument::AddZoneWithIdToWorld(%d)\n", zoneID);
    chosen_zone_ids.insert(chosen_zone_ids.begin(), zoneID);
}

uint16 YodaDocument::getZoneID(Zone *zone){
    for(int i=0; i < zones.size(); i++)
        if(zones[i] == zone) return i;
    return -1;
}

uint16 YodaDocument::getZoneIDAt(int x, int y){
    return world_things[x + 10 * y].zone_id;
}

Quest* YodaDocument::AddProvidedQuestWithItemID(uint16 itemID, uint16 maximumDistance){
    printf("YodaDocument::AddProvidedQuestWithItemID(%d, %d)\n", itemID, maximumDistance);
    for(Quest *quest : providedItems)
        if(quest->itemID == itemID) return quest;
    
    providedItems.insert(providedItems.begin() , new Quest(itemID, maximumDistance));
    return providedItems[providedItems.size()-1];
}

Quest* YodaDocument::AddRequiredQuestWithItemID(uint16 itemID, uint16 maximumDistance){
    printf("YodaDocument::AddRequiredQuestWithItemID(%d, %d)\n", itemID, maximumDistance);
    //    for(Quest *quest : requiredItems)
    //        if(quest->itemID == itemID) return quest;
    
    lastAddedQuestItemId = itemID;
    requiredItems.push_back(new Quest(itemID, maximumDistance));
    return requiredItems[requiredItems.size()-1];
}

void YodaDocument::RemoveQuestProvidingItem(uint16 itemID) {
    printf("YodaDocument::RemoveQuestProvidingItem(%d)\n", itemID);
    for(int i=0; i < providedItems.size(); i++)
        if(providedItems[i]->itemID == itemID) {
            providedItems.erase(providedItems.begin()+i);
            return;
        }
}

void YodaDocument::RemoveQuestRequiringItem(uint16 itemID) {
    printf("YodaDocument::RemoveQuestRequiringItem(%d)\n", itemID);
    for(int i=0; i < requiredItems.size(); i++)
        if(requiredItems[i]->itemID == itemID) {
            requiredItems.erase(requiredItems.begin()+i);
            return;
        }
}

int YodaDocument::HasQuestRequiringItem(uint16 itemID) {
    printf("YodaDocument::HasQuestRequiringItem(%d)\n", itemID);
    for(Quest *quest : requiredItems)
        if(quest->itemID == itemID) return 1;
    
    return 0;
}

int YodaDocument::GetLocationOfZoneWithID(uint16 zoneID, int *xOut, int *yOut) {
    for(int y=0; y < 10; y++)
        for(int x=0; x < 10; x++)
            if(world_things[x + 10 * y].zone_id == zoneID) {
                *xOut = x;
                *yOut = y;
                return 1;
            }
    
    return 0;
}

int YodaDocument::AddRequiredItemsFromHotspots(uint16 zoneID) {
    printf("YodaDocument::AddRequiredItemsFromHotspots(%d)\n", zoneID);
    Zone* zone = zones[zoneID];
    for (Hotspot* hotspot : zone->_hotspots) {
        switch (hotspot->type) {
            case CrateItem:
            case PuzzleNPC:
            case CrateWeapon:
                if((hotspot->arg1 & 0x8000) == 0)
                    AddRequiredQuestWithItemID(hotspot->arg1, -1);
                break;
            case DoorIn:
                if((hotspot->arg1 & 0x8000) == 0)
                    AddRequiredItemsFromHotspots(hotspot->arg1);
                break;
            default:
                break;
        }
    }
    return 0;
}

int YodaDocument::ZoneLeadsToItem(uint16 zoneID, uint16 itemID) {
    printf("YodaDocument::ZoneLeadsToItem(%d, %d)\n", zoneID, itemID);
    Zone *zone = getZoneByID(zoneID);
    for(uint16 itemIDInZone : zone->providedItemIDs)
        if(itemIDInZone == itemID) return 1;
    
    for(Hotspot* hotspot : zone->_hotspots)
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0 && ZoneLeadsToItem(hotspot->arg1, itemID))
            return 1;
    
    return 0;
}

int YodaDocument::GetItemIDThatsNotRequiredYet(__int16 zone_id, int unused, int use_array_2_ids)
{
    printf("YodaDocument::GetItemIDThatsNotRequiredYet(%d, %d, %d)\n", zone_id, unused, use_array_2_ids);
    vector<uint16> itemIDs;
    Zone *zone = zones[zone_id];
    
    vector<uint16>* zoneItemIds = use_array_2_ids ? &(zone->assignedItemIDs) : &(zone->requiredItemIDs);
    for(uint16 itemID : *zoneItemIds) {
        if(!HasQuestRequiringItem(itemID)) {
            itemIDs.push_back(itemID);
        }
    }
    
    if(itemIDs.size()){
        return itemIDs[win_rand() % itemIDs.size()];
    }
    
    for(Hotspot *hotspot : zone->_hotspots){
        if(hotspot->type == DoorIn) {
            uint16 itemID = GetItemIDThatsNotRequiredYet(hotspot->arg1, unused, use_array_2_ids);
            if(itemID >= 0) return itemID;
        }
    }
    
    return -1;
}
#pragma mark -
signed int YodaDocument::GenerateWorld(int seed, int puzzle_count, int16* map, int16 *puzzleMap) {
    return 0;
}

signed int YodaDocument::ChooseItemIDFromZone(__int16 zone_id, int item_id, int a4)
{
    printf("YodaDocument::ChooseItemIDFromZone(%d, %d, %d)\n", zone_id, item_id, a4);
    signed int v4; // esi@1
    Zone *zone; // eax@1
    Zone *zone_1; // edi@1
    int v8; // eax@4
    int v9; // ecx@4
    __int16 v11; // dx@6
    int v12; // eax@9
    int v13; // ecx@9
    int v15; // ebx@15
    int v16; // ebp@17
    Hotspot *v17; // eax@18
    __int16 v18; // cx@20
    int v19; // [sp+10h] [bp-8h]@16
    
    v4 = -1;
    zone = getZoneByID(zone_id);
    zone_1 = zone;
    if ( !zone )
        return -1;
    if ( a4 )
    {
        v8 = (int)zone->assignedItemIDs.size();
        v9 = 0;
        if ( v8 <= 0 )
            goto LABEL_15;
        int idx = 0;
        while ( 1 )
        {
            v11 = zone_1->assignedItemIDs[idx];
            if ( item_id == zone_1->assignedItemIDs[idx] )
                break;
            ++idx;
            if ( ++v9 >= v8 )
                goto LABEL_15;
        }
    }
    else
    {
        v12 = (int)zone->requiredItemIDs.size();
        v13 = 0;
        if ( v12 <= 0 )
            goto LABEL_15;
        int idx = 0;
        while ( 1 )
        {
            v11 = zone_1->requiredItemIDs[idx];
            if ( item_id == zone_1->requiredItemIDs[idx] )
                break;
            ++idx;
            if ( ++v13 >= v12 )
                goto LABEL_15;
        }
    }
    v4 = v11;
LABEL_15:
    v15 = 0;
    if ( v4 < 0 )
    {
        v19 = (int)zone_1->_hotspots.size();
        if ( v19 > 0 )
        {
            v16 = 0;
            do
            {
                v17 = zone_1->_hotspots[v16];
                if ( v17 && v17->type == 9 )
                {
                    v18 = v17->arg1;
                    if ( v18 >= 0 )
                        v4 = ChooseItemIDFromZone(v18, item_id, a4);
                    if ( v4 >= 0 )
                        break;
                }
                ++v16;
                ++v15;
            }
            while ( v19 > v15 );
        }
    }
    return v4;
}

signed int YodaDocument::ChooseItemIDFromZone_0(__int16 zone_id, int item_id)
{
    printf("YodaDocument::ChooseItemIDFromZone_0(%d, %d)\n", zone_id, item_id);
    
    Zone *zone_1; // eax@1
    Zone *zone; // esi@1
    signed int result; // eax@2
    int v7; // ebx@7
    int idx; // edi@7
    Hotspot *v9; // ecx@12
    int v10; // ebx@18
    int v11; // edi@19
    Hotspot *v12; // eax@20
    __int16 zone_id_1; // cx@22
    vector<uint16> v14; // [sp+Ch] [bp-38h]@1
    int v16 = 0; // [sp+14h] [bp-30h]@9
    unsigned int v17; // [sp+20h] [bp-24h]@5
    int v18; // [sp+24h] [bp-20h]@4
    int v19; // [sp+28h] [bp-1Ch]@4
    int v21; // [sp+30h] [bp-14h]@1
    int v22; // [sp+34h] [bp-10h]@18
    __int16 v23; // [sp+36h] [bp-Eh]@6
    int v24; // [sp+40h] [bp-4h]@1
    
    v21 = -1;
    v14.clear();
    v24 = 0;
    zone_1 = getZoneByID(zone_id);
    zone = zone_1;
    if ( zone_1 )
    {
        printf("v16 = %d\n", v16);
        v18 = 0;
        v19 = (int)zone_1->providedItemIDs.size();
        if ( v19 > 0 )
        {
            v17 = 0;
            do
            {
                v23 = zone->providedItemIDs[v17 / 2];
                if ( item_id == v23 )
                {
                    v7 = (int)zone->_hotspots.size();
                    idx = 0;
                    v14.clear();
                    if ( v7 > 0 )
                    {
                        do
                        {
                            if ( zone->_hotspots[idx]->type == TriggerLocation ) {
                                v14.insert(v14.begin() + v16, idx);
                                v16++;
                            }
                            ++idx;
                        }
                        while ( v7 > idx );
                    }
                    if ( v16 > 0 )
                    {
                        v9 = zone->_hotspots[v14[(win_rand() % v16)]];
                        if ( v9 )
                        {
                            if ( v9->type == TriggerLocation )
                            {
                                v21 = v23;
                                v9->arg1 = v23;
                                v9->enabled = 1;
                            }
                        }
                    }
                    if ( v21 >= 0 )
                        break;
                }
                v17 += 2;
                ++v18;
            }
            while ( v18 < v19 );
        }
        if ( v21 == -1 )
        {
            v10 = 0;
            v22 = (int)zone->_hotspots.size();
            if ( v22 > 0 )
            {
                v11 = 0;
                do
                {
                    v12 = zone->_hotspots[v11];
                    if ( v12 && v12->type == DoorIn )
                    {
                        zone_id_1 = v12->arg1;
                        if ( zone_id_1 >= 0 )
                            v21 = ChooseItemIDFromZone_0(zone_id_1, item_id);
                        if ( v21 >= 0 ) {
                            break;
                        }
                    }
                    ++v11;
                    ++v10;
                }
                while ( v22 > v10 );
            }
        }
        v24 = -1;
        result = v21;
    }
    else
    {
        v24 = -1;
        result = -1;
    }
    return result;
}

signed int YodaDocument::ChooseItemIDFromZone_1(__int16 a2, int a3, int a4, __int16 a5, int a6)
{
    printf("YodaDocument::ChooseItemIDFromZone_1(%d, %d, %d, %d, %d)\n", a2, a3, a4, a6, a6);
    
    signed int result; // eax@1
    signed int v7; // ebx@1
    Zone *zone; // esi@2
    int itemID; // ecx@3
    int v10; // ebp@3
    __int16 item_id; // di@4
    int v13; // ecx@9
    int v14; // ebp@9
    int v16; // ebp@17
    int v17; // ecx@17
    Hotspot *v19; // ebx@19
    int v20; // ebx@26
    int v21; // ebp@27
    Hotspot *v22; // ecx@28
    int v24; // [sp+14h] [bp-4h]@27
    
    result = 0;
    v7 = 0;
    if ( a2 < 0 )
        return result;
    zone = this->zones[a2];
    if ( a6 )
    {
        itemID = (int)zone->assignedItemIDs.size();
        v10 = 0;
        if ( itemID > 0 )
        {
            item_id = a5;
            int idx = 0;
            while ( zone->assignedItemIDs[idx] != a5 )
            {
                ++idx;
                if ( ++v10 >= itemID )
                    goto LABEL_16;
            }
            v7 = 1;
            goto LABEL_16;
        }
    }
    else
    {
        v13 = (int)zone->requiredItemIDs.size();
        v14 = 0;
        if ( v13 > 0 )
        {
            item_id = a5;
            int idx = 0;
            while ( zone->requiredItemIDs[idx] != a5 )
            {
                ++idx;
                if ( ++v14 >= v13 )
                    goto LABEL_16;
            }
            v7 = 1;
            goto LABEL_16;
        }
    }
    item_id = a5;
LABEL_16:
    if ( v7 )
    {
        v16 = (int)zone->_hotspots.size();
        v17 = 0;
        if ( v16 > 0 )
        {
            int idx = 0;
            while ( 1 )
            {
                v19 = zone->_hotspots[idx];
                if ( zone->_hotspots[idx]->type == Lock )
                    break;
                ++idx;
                if ( ++v17 >= v16 )
                    goto LABEL_26;
            }
            AddRequiredQuestWithItemID(item_id, a4);
            if ( a6 )
                this->wg_item_id_unknown_3 = item_id;
            else
                this->wg_item_id = item_id;
            
            v19->arg1 = item_id;
            result = 1;
            v19->enabled = 1;
        }
    }
LABEL_26:
    v20 = 0;
    if ( !result )
    {
        v21 = 0;
        v24 = (int)zone->_hotspots.size();
        if ( v24 > 0 )
        {
            do
            {
                v22 = zone->_hotspots[v20];
                if ( v22->type == 9 )
                {
                    result = ChooseItemIDFromZone_1(v22->arg1, a3, a4, item_id, a6);
                    if ( result == 1 )
                        break;
                }
                ++v20;
                ++v21;
            }
            while ( v21 < v24 );
        }
    }
    return result;
}

signed int YodaDocument::ChooseItemIDFromZone_2(__int16 zone_id, __int16 a3, int a4)
{
    printf("YodaDocument::ChooseItemIDFromZone_2(%d, %d, %d)\n", zone_id, a3, a4);
    
    Zone *zone; // eax@1
    Zone *zone_1; // edi@1
    int v7; // eax@4
    int v8; // ecx@4
    __int16 v9; // si@5
    int v11; // eax@10
    int v12; // ecx@10
    int i; // ebx@17
    int offset; // ebp@19
    Hotspot *hotspot; // eax@20
    __int16 v17; // cx@22
    signed int v18; // [sp+10h] [bp-Ch]@1
    int hotspot_count; // [sp+14h] [bp-8h]@18
    
    v18 = 0;
    zone = getZoneByID(zone_id);
    zone_1 = zone;
    if ( !zone )
        return 0;
    if ( a4 )
    {
        v7 = (int)zone->assignedItemIDs.size();
        v8 = 0;
        if ( v7 > 0 )
        {
            v9 = a3;
            int idx = 0;
            while ( zone_1->assignedItemIDs[idx] != a3 )
            {
                ++idx;
                if ( ++v8 >= v7 )
                    goto LABEL_17;
            }
            v18 = 1;
            goto LABEL_17;
        }
    }
    else
    {
        v11 = (int)zone->requiredItemIDs.size();
        v12 = 0;
        if ( v11 > 0 )
        {
            v9 = a3;
            int idx = 0;
            while ( zone_1->requiredItemIDs[idx] != a3 )
            {
                ++idx;
                if ( ++v12 >= v11 )
                    goto LABEL_17;
            }
            v18 = 1;
            goto LABEL_17;
        }
    }
    v9 = a3;
LABEL_17:
    i = 0;
    if ( !v18 )
    {
        hotspot_count = (int)zone_1->_hotspots.size();
        if ( hotspot_count > 0 )
        {
            offset = 0;
            do
            {
                hotspot = zone_1->_hotspots[offset];
                if ( hotspot && hotspot->type == DoorIn )
                {
                    v17 = hotspot->arg1;
                    if ( v17 >= 0 )
                        v18 = ChooseItemIDFromZone_2(v17, v9, a4);
                    if ( v18 == 1 )
                        break;
                }
                ++offset;
                ++i;
            }
            while ( hotspot_count > i );
        }
    }
    return v18;
}

signed int YodaDocument::ChoosePuzzleNPCForZone(__int16 zone_id)
{
    printf("YodaDocument::ChoosePuzzleNPCForZone(%d)\n", zone_id);
    Zone *zone = getZoneByID(zone_id);
    if (!zone) return -1;
    
    vector<uint16> v13;
    for(int i=0; i < zone->puzzleNPCTileIDs.size(); i++) {
        if(!HasQuestRequiringItem(zone->puzzleNPCTileIDs[i])) {
            v13.push_back(i);
        }
    }
    
    if(v13.size()) {
        return zone->puzzleNPCTileIDs[v13[win_rand() % v13.size()]];
    }
    
    for(Hotspot *hotspot : zone->_hotspots) {
        if(hotspot->type == DoorIn && hotspot->arg1 >= 0) {
            int result = ChoosePuzzleNPCForZone(hotspot->arg1);
            if(result >= 0) return result;
        }
    }
    
    return -1;
}

signed int YodaDocument::ChooseSpawnForPuzzleNPC(__int16 a2, int a3)
{
    printf("YodaDocument::ChooseSpawnForPuzzleNPC(%d, %d)\n", a2, a3);
    Zone *zone_1; // eax@1
    Zone *zone; // esi@1
    signed int result; // eax@2
    int v7; // ebx@7
    int v8; // edi@7
    Hotspot *hotspot; // ecx@12
    int v10; // edx@14
    int v11; // ebx@18
    int v12; // edi@19
    Hotspot *v13; // eax@20
    __int16 v14; // ax@22
    vector<uint16> v15; // [sp+Ch] [bp-38h]@1
    int v17 = 0; // [sp+14h] [bp-30h]@9
    unsigned int v18; // [sp+20h] [bp-24h]@5
    int v19; // [sp+24h] [bp-20h]@4
    int v20; // [sp+28h] [bp-1Ch]@4
    int v22; // [sp+30h] [bp-14h]@1
    int v23; // [sp+34h] [bp-10h]@18
    __int16 v24; // [sp+36h] [bp-Eh]@6
    int v25; // [sp+40h] [bp-4h]@1
    
    v22 = -1;
    v25 = 0;
    zone_1 = getZoneByID(a2);
    zone = zone_1;
    if ( zone_1 )
    {
        v19 = 0;
        v20 = (int)zone_1->puzzleNPCTileIDs.size();
        if ( v20 > 0 )
        {
            v18 = 0;
            do
            {
                v24 = zone->puzzleNPCTileIDs[v18 / 2];
                if ( a3 == v24 )
                {
                    v7 = (int)zone->_hotspots.size();
                    v8 = 0;
                    v15.clear();
                    if ( v7 > 0 )
                    {
                        do
                        {
                            if ( zone->_hotspots[v8]->type == SpawnLocation ) {
                                v15.insert(v15.begin()+v17, v8);
                                v17++;
                            }
                            ++v8;
                        }
                        while ( v7 > v8 );
                    }
                    
                    if ( v17 > 0 )
                    {
                        hotspot = zone->_hotspots[v15[win_rand() % v17]];
                        if ( hotspot )
                        {
                            if ( hotspot->type == SpawnLocation )
                            {
                                v10 = v24;
                                v22 = v24;
                                hotspot->arg1 = v24;
                                hotspot->enabled = 1;
                                this->field_3390 = v10;
                            }
                        }
                    }
                    if ( v22 >= 0 ) return v22;
                }
                v18 += 2;
                ++v19;
            }
            while ( v19 < v20 );
        }
        if ( v22 == -1 )
        {
            v11 = 0;
            v23 = (int)zone->_hotspots.size();
            if ( v23 > 0 )
            {
                v12 = 0;
                do
                {
                    v13 = zone->_hotspots[v12];
                    if ( v13 && v13->type == 9 )
                    {
                        v14 = v13->arg1;
                        if ( v14 >= 0 ) v22 = ChooseSpawnForPuzzleNPC(v14, a3);
                        if ( v22 >= 0 ) return v22;
                    }
                    ++v12;
                    ++v11;
                }
                while ( v23 > v11 );
            }
        }
        v25 = -1;
        result = v22;
    }
    else
    {
        v25 = -1;
        result = -1;
    }
    return result;
}

signed int YodaDocument::use_ids_from_array_1(__int16 zone_id, __int16 a3, __int16 item_id_1, __int16 a5)
{
    printf("YodaDocument::use_ids_from_array_1(%d, %d, %d, %d)\n", zone_id, a3, item_id_1, a5);
    Zone *zone; // eax@3
    __int16 v8; // cx@10
    vector<uint16> *v9; // eax@14
    __int16 v10; // dx@17
    vector<Puzzle*>* v11; // ebx@19
    Puzzle *v12; // ecx@19
    Puzzle *v13; // eax@22
    __int16 item_id; // bp@25
    signed int v15; // ebx@27
    __int16 a3a; // [sp+12h] [bp-Ah]@25
    Puzzle *a3_2; // [sp+14h] [bp-8h]@0
    
    if ( zone_id < 0 )
        return 0;
    zone = this->zones[zone_id];
    if ( !zone )
        return 0;
    if ( zone->getType() != ZONETYPE_Use )
        return 0;
    if ( !ZoneDoesNOTProvideRequiredItemID(zone_id) )
        return 0;
    if ( a5 )
        v8 = this->puzzle_ids_1[a3];
    else
        v8 = this->puzzle_ids_2[a3];
    if ( a5 )
    {
        if ( this->puzzle_ids_1.size() - 1 > a3 )
        {
            v9 = &this->puzzle_ids_1;
            goto LABEL_17;
        }
    }
    else if ( this->puzzle_ids_2.size() - 1 > a3 )
    {
        v9 = &this->puzzle_ids_2;
    LABEL_17:
        v10 = (*v9)[a3 + 1];
        goto LABEL_19;
    }
    v10 = -1;
LABEL_19:
    v11 = &this->puzzles;
    v12 = (*v11)[v8];
    if ( !v12 )
        return 0;
    if ( v10 < 0 )
    {
        v13 = a3_2;
    }
    else
    {
        v13 = (*v11)[v10];
        if ( !v13 )
            return 0;
    }
    item_id = -1;
    a3a = v12->item_1;
    if ( v10 >= 0 )
        item_id = v13->item_1;
    v15 = ChoosePuzzleNPCForZone(zone_id);
    if ( v15 < 0 ) return 0;
    
    int v19 = 1;
    int a3_2a = ChooseItemIDFromZone_2(zone_id, a3a, 0);
    if ( item_id >= 0 )
        v19 = ZoneLeadsToItem(zone_id, item_id);
    
    if ( !a3_2a || !v19 ) return 0;
    
    if ( ChooseSpawnForPuzzleNPC(zone_id, v15) < 0 ) return 0;
    
    this->field_3390 = v15;
    this->wg_last_added_item_id = item_id;
    this->wg_item_id = a3a;
    this->field_3394 = a3;
    
    AddRequiredQuestWithItemID(v15, item_id_1);
    AddRequiredItemsFromHotspots(zone_id);
    
    return 1;
}

signed int YodaDocument::ChoosePuzzleNPCForZone_0(__int16 zone_id, __int16 unknown)
{
    printf("YodaDocument::ChoosePuzzleNPCForZone_0(%d, %d)\n", zone_id, unknown);
    signed int v3; // edi@1
    Zone *v4; // eax@1
    Zone *v5; // ebx@1
    signed int result; // eax@2
    int v7; // eax@3
    bool v8; // zf@4
    bool v9; // sf@4
    int v10; // ecx@6
    int v12; // ebp@12
    Hotspot *hotspot; // eax@14
    __int16 zone_id_2; // ax@16
    int v15; // [sp+10h] [bp-Ch]@13
    int v16; // [sp+14h] [bp-8h]@13
    
    v3 = 0;
    v4 = getZoneByID(zone_id);
    v5 = v4;
    if ( v4 )
    {
        v7 = (int)v4->puzzleNPCTileIDs.size();
        if ( unknown == -1 )
        {
            v8 = v7 == 0;
            v9 = v7 < 0;
            result = 1;
            if ( v9 || v8 )
                result = 0;
        }
        else
        {
            v10 = 0;
            if ( v7 > 0 )
            {
                int idx = 0;
                while ( v5->puzzleNPCTileIDs[idx] != unknown )
                {
                    ++idx;
                    if ( v7 <= ++v10 )
                        goto LABEL_12;
                }
                v3 = 1;
            }
        LABEL_12:
            v12 = 0;
            if ( !v3 )
            {
                v15 = 0;
                v16 = (int)v5->_hotspots.size();
                if ( v16 > 0 )
                {
                    do
                    {
                        hotspot = v5->_hotspots[v12];
                        if ( hotspot && hotspot->type == DoorIn )
                        {
                            zone_id_2 = hotspot->arg1;
                            if ( zone_id_2 >= 0 )
                                v3 = ChoosePuzzleNPCForZone_0(zone_id_2, unknown);
                            if ( v3 == 1 )
                                break;
                        }
                        ++v12;
                        ++v15;
                    }
                    while ( v16 > v15 );
                }
            }
            result = v3;
        }
    }
    else
    {
        result = 0;
    }
    return result;
}

int YodaDocument::Unknown_7(__int16 zone_id, __int16 puzzle_idx, __int16 a4, int unknown, int a6)
{
    printf("YodaDocument::Unknown_7(%d, %d, %d, %d, %d)\n", zone_id, puzzle_idx, a4, unknown, a6);
    Zone *zone; // eax@1
    signed int result; // eax@2
    uint16 item; // ebx@5
    signed int v14; // eax@7
    int v15; // ebp@7
    signed int v16; // ST28_4@9
    signed int v17; // ebp@15
    signed int v19; // eax@18
    signed int v21; // [sp+18h] [bp-14h]@18
    signed int v22; // [sp+1Ch] [bp-10h]@15
    signed int v23; // [sp+20h] [bp-Ch]@9
    signed int v24; // [sp+20h] [bp-Ch]@18
    
    zone = this->zones[zone_id];
    if (!zone) return 0;
    
    if ( zone->getType() != ZONETYPE_Goal ) return 0;
    
    if (!ZoneDoesNOTProvideRequiredItemID(zone_id) )
        return 0;
    
    Puzzle *puzzle1 = puzzles[this->puzzle_ids_1[puzzle_idx]];
    Puzzle *puzzle3 = puzzles[this->puzzle_ids_1[puzzle_idx+1]];
    Puzzle *puzzle2 = puzzles[this->puzzle_ids_2[a4]];
    
    puzzle1->unknown_3 = 0;
    v14 = ChoosePuzzleNPCForZone(zone_id);
    v15 = v14;
    if ( v14 >= 0 )
        puzzle1->unknown_3 = ChoosePuzzleNPCForZone_0(zone_id, v14);
    
    v16 = ChooseItemIDFromZone_2(zone_id, puzzle1->item_1, 0);
    ChooseItemIDFromZone_2(zone_id, puzzle2->item_1, 1);
    v23 = ZoneLeadsToItem(zone_id, puzzle3->item_1);
    ZoneLeadsToItem(zone_id, puzzle3->item_2);
    if ( !v16 || !v23 )
        return 0;
    
    if ( puzzle1->unknown_3 ) {
        if ( AddRequiredItemsFromHotspots(zone_id) < 0 ) {
            result = 0;
        } else {
            this->field_3390 = v15;
            this->wg_last_added_item_id = item;
            this->wg_item_id_unknown_2 = puzzle1->item_1;
            this->wg_item_id = puzzle1->item_1;
            this->field_3394 = puzzle_idx;
            this->field_3398 = a4;
            this->wg_item_id_unknown_3 = puzzle1->item_1;
            AddRequiredItemsFromHotspots(zone_id);
            result = 1;
        }
    } else {
        v17 = ChooseItemIDFromZone(zone_id, puzzle1->item_1, 0);
        v22 = ChooseItemIDFromZone_0(zone_id, puzzle3->item_1);
        if ( v17 >= 0 && v22 >= 0 ) {
            AddRequiredQuestWithItemID(puzzle1->item_1, unknown);
            AddRequiredQuestWithItemID(puzzle3->item_1, unknown);
        }
        v24 = ChooseItemIDFromZone(zone_id, puzzle2->item_1, 1);
        v19 = ChooseItemIDFromZone_0(zone_id, puzzle3->item_2);
        v21 = v19;
        if ( v24 >= 0 && v19 >= 0 ) {
            AddRequiredQuestWithItemID(puzzle2->item_1, unknown);
            AddRequiredQuestWithItemID(puzzle3->item_2, unknown);
        }
        if ( v17 < 0 || v22 < 0 || v24 < 0 || v21 < 0 ) {
            RemoveQuestRequiringItem(puzzle1->item_1);
            RemoveQuestRequiringItem(item);
            RemoveQuestRequiringItem(puzzle1->item_1);
            RemoveQuestRequiringItem(puzzle1->item_1);
            result = 0;
        } else {
            this->field_3390 = -1;
            this->wg_last_added_item_id = item;
            this->wg_item_id = puzzle1->item_1;
            this->field_3394 = puzzle_idx;
            this->wg_item_id_unknown_2 = puzzle1->item_1;
            this->wg_item_id_unknown_3 = puzzle1->item_1;
            this->field_3398 = a4;
            AddRequiredItemsFromHotspots(zone_id);
            result = 1;
        }
    }
    
    return result;
}

signed int YodaDocument::ZoneDoesNOTProvideRequiredItemID(__int16 zone_id)
{
    printf("YodaDocument::ZoneDoesNOTProvideRequiredItemID(%d)\n", zone_id);
    signed int result_1; // esi@1
    Zone *v3; // ebx@1
    int count_1; // ebp@3
    int hotspot_offset; // edi@4
    Hotspot *hotspot; // ecx@5
    unsigned int hotspot_type; // eax@5
    __int16 item_id; // ax@9
    __int16 item_id_1; // ax@12
    int count; // [sp+14h] [bp-4h]@3
    
    result_1 = 1;
    v3 = this->zones[zone_id];
    if ( !v3 ) return 0;
    
    count_1 = 0;
    count = (int)v3->_hotspots.size();
    if ( count > 0 )
    {
        hotspot_offset = 0;
        do
        {
            hotspot = v3->_hotspots[hotspot_offset];
            hotspot_type = hotspot->type;
            if ( hotspot_type >= CrateItem )
            {
                if ( hotspot_type <= CrateWeapon )
                {
                    item_id = hotspot->arg1;
                    if ( item_id >= 0 && HasQuestRequiringItem(item_id) )
                        result_1 = 0;
                }
                else if ( hotspot_type == DoorIn )
                {
                    item_id_1 = hotspot->arg1;
                    if ( item_id_1 >= 0 )
                        result_1 = ZoneDoesNOTProvideRequiredItemID(item_id_1);
                }
            }
            if ( !result_1 )
                break;
            ++hotspot_offset;
            ++count_1;
        }
        while ( count > count_1 );
    }
    
    return result_1;
}

signed int YodaDocument::SetupRequiredItemForZone_(__int16 zone_id, __int16 arg2, int use_required_items_array)
{
    printf("YodaDocument::SetupRequiredItemForZone_(%d, %d, %d)\n", zone_id, arg2, use_required_items_array);
    int count; // esi@6
    int idx; // edi@12
    uint16 item_id; // bx@13
    int idx_1; // edi@18
    uint16 v10; // bx@19
    int count_2; // esi@23
    int random_item_id; // esi@23
    signed int v14; // ebx@24
    int count_1; // ecx@24
    __int16 zone_id_1; // di@32
    vector<uint16> item_ids_1; // [sp+Ch] [bp-28h]@1
    Zone *zone; // [sp+20h] [bp-14h]@4
    int v22; // [sp+30h] [bp-4h]@1
    
    item_ids_1.clear();
    
    v22 = 0;
    if ( zone_id < 0 ){
        v22 = -1;
        return 0;
    }
    
    zone = this->zones[zone_id];
    if ( !zone )
    {
        v22 = -1;
        return 0;
    }
    
    count = use_required_items_array ? (int)zone->assignedItemIDs.size() : (int)zone->requiredItemIDs.size();
    if ( !count || !ZoneDoesNOTProvideRequiredItemID(zone_id))
    {
        v22 = -1;
        return 0;
    }
    
    if ( use_required_items_array )
    {
        if ( count > 0 )
        {
            idx = 0;
            do
            {
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
    else if ( count > 0 )
    {
        idx_1 = 0;
        do
        {
            v10 = zone->requiredItemIDs[idx_1];
            if ( !HasQuestRequiringItem(zone->requiredItemIDs[idx_1]) )
                item_ids_1.push_back(v10);
            ++idx_1;
            --count;
        }
        while ( count );
    }
    
    if ( !item_ids_1.size() ) {
        v22 = -1;
        return 0;
    }
    
    count_2 = (int)item_ids_1.size();
    random_item_id = item_ids_1[win_rand() % count_2];
    if ( zone->providedItemIDs.size() == 1 )
    {
        int idx = 0;
        v14 = 0;
        count_1 = (int)this->item_ids.size();
        if ( count_1 > 0 )
        {
            int documentItemIdx = 0;
            do
            {
                if ( this->item_ids[documentItemIdx] == zone->providedItemIDs[idx])
                    v14 = 1;
                ++documentItemIdx;
                --count_1;
            }
            while ( count_1 );
        }
        if ( !v14 )
        {
            this->item_ids.push_back(zone->providedItemIDs[idx]);
            goto LABEL_31;
        }
    return_0:
        v22 = -1;
        return 0;
    }
LABEL_31:
    if ( zone->getType() == ZONETYPE_TravelStart )
    {
        AddProvidedQuestWithItemID(random_item_id, 5);
        zone_id_1 = arg2;
    }
    else
    {
        zone_id_1 = arg2;
        AddProvidedQuestWithItemID(random_item_id, arg2);
    }
    AddRequiredQuestWithItemID(random_item_id, zone_id_1);
    // document_1 = document;
    this->wg_item_id = random_item_id;
    AddRequiredItemsFromHotspots(zone_id);
    v22 = -1;
    
    return 1;
}

int YodaDocument::AssociateItemWithZoneHotspot(__int16 zone_id, int item_id, int a4)
{
    printf("YodaDocument::AssociateItemWithZoneHotspot(%d, %d, %d)\n", zone_id, item_id, a4);
    signed int found_item_id_in_zone_items; // esi@1
    Zone *zone; // edi@3
    int item_ids; // eax@6
    int v8; // ebx@6
    int hotspot_type; // ebx@12
    int tile_specs; // eax@13
    int idx; // esi@19
    int v13; // esi@24
    Hotspot *hotspot; // eax@24
    int idx_1; // esi@26
    int v16; // ebx@26
    Hotspot *hotspot_1; // eax@27
    int zone_id_1; // edx@28
    vector<uint16> v20; // [sp+Ch] [bp-30h]@13
    int hotspot_indexes; // [sp+14h] [bp-28h]@21
    int item_id_1; // [sp+20h] [bp-1Ch]@13
    int v24; // [sp+24h] [bp-18h]@19
    int didFindSuitableHotspot; // [sp+28h] [bp-14h]@1
    int v27; // [sp+38h] [bp-4h]@13
    
    found_item_id_in_zone_items = 0;
    didFindSuitableHotspot = 0;
    if ( zone_id >= 0 )
    {
        if ( ZoneDoesNOTProvideRequiredItemID(zone_id) )
        {
            zone = this->zones[zone_id];
            if ( zone )
            {
                if ( zone->requiredItemIDs.size() <= 0 && zone->puzzleNPCTileIDs.size() <= 0 )
                {
                    item_ids = (int)zone->providedItemIDs.size();
                    v8 = 0;
                    if ( item_ids > 0 )
                    {
                        int idx = 0;
                        while ( zone->providedItemIDs[idx] != item_id )
                        {
                            ++idx;
                            if ( item_ids <= ++v8 )
                                goto LABEL_12;
                        }
                        found_item_id_in_zone_items = 1;
                    }
                LABEL_12:
                    hotspot_type = 0;
                    if ( found_item_id_in_zone_items )
                    {
                        v20.clear();
                        v20.resize(0);
                        v27 = 0;
                        item_id_1 = item_id;
                        tile_specs = tiles[item_id]->_specs;
                        if ( tile_specs & TILE_SPEC_THE_FORCE )
                        {
                            hotspot_type = ForceLocation;
                        }
                        else if ( tile_specs & TILE_SPEC_MAP )
                        {
                            hotspot_type = LocatorThingy;
                        }
                        else if ( (uint8_t)tile_specs & (uint8_t)TILE_SPEC_USEFUL )
                        {
                            hotspot_type = TriggerLocation;
                        }
                        idx = 0;
                        v24 = (int)zone->_hotspots.size();
                        if ( v24 > 0 )
                        {
                            do
                            {
                                if ( zone->_hotspots[idx]->type == hotspot_type )
                                {
                                    v20.push_back(idx);
                                    didFindSuitableHotspot = 1;
                                }
                                ++idx;
                            }
                            while ( v24 > idx );
                        }
                        if ( didFindSuitableHotspot == 1 )
                        {
                            v13 = hotspot_indexes;
                            hotspot = zone->_hotspots[v20[win_rand() % v20.size()]];
                            hotspot->arg1 = item_id;
                            hotspot->enabled = 1;
                            AddRequiredQuestWithItemID(item_id, a4);
                            wg_last_added_item_id = item_id_1;
                        }
                        v27 = -1;
                    }
                    else
                    {
                        idx_1 = 0;
                        v16 = 0;
                        v24 = (int)zone->_hotspots.size();
                        if ( v24 > 0 )
                        {
                            do
                            {
                                hotspot_1 = zone->_hotspots[idx_1];
                                if ( hotspot_1->type == DoorIn )
                                {
                                    zone_id_1 = hotspot_1->arg1;
                                    if ( zone_id_1 >= 0 )
                                        didFindSuitableHotspot = AssociateItemWithZoneHotspot(zone_id_1, item_id, a4);
                                    if ( didFindSuitableHotspot == 1 )
                                        goto LABEL_33;
                                }
                                ++idx_1;
                            }
                            while ( v24 > ++v16 );
                        }
                    }
                    if ( didFindSuitableHotspot != 1 )
                        return didFindSuitableHotspot;
                LABEL_33:
                    AddRequiredItemsFromHotspots(zone_id);
                    return didFindSuitableHotspot;
                }
            }
        }
    }
    return 0;
}

int16 YodaDocument::GetZoneIdWithType(ZONE_TYPE type_1, int a3, int a4, int item_id_1, int item_id_2, __int16 item_id_3, int a8) {
    printf("YodaDocument::GetZoneIdWithType(%d, %d, %d, %d, %d, %d, %d)\n", type_1, a3, a4, item_id_1, item_id_2, item_id_3, a8);
    
    // item_id_1 = first required quest->itemID, last required quest->itemID
    vector<int16> usable_zone_ids;
    for(int zone_index=0; zone_index < this->zones.size(); zone_index++) {
        Zone *zone = this->zones[zone_index];
        if ( (size_t)zone == -1 || !zone || zone->getPlanet() != this->planet )
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
    
    int v42 = 0;
    if (usable_zone_ids.size() == 0) {
        v42 = -1;
        return -1;
    }
    
    ShuffleVector(usable_zone_ids);
    
    for(int idx=0; idx < usable_zone_ids.size(); idx++){
        Zone *zone_1; // esi@17
        int count_1; // edx@24
        __int16 v20; // eax@50
        int v21; // eax@52
        int v22; // esi@52
        int puzzle_id; // eax@54
        int v24; // esi@58
        int v25; // eax@65
        int16 v26; // si@65
        int zone_type; // eax@80
        unsigned int v33; // [sp+20h] [bp-2Ch]@58
        __int16 item_id[2]; // [sp+28h] [bp-24h]@50
        __int16 item_ids; // [sp+2Ch] [bp-20h]@50
        __int16 v38; // [sp+34h] [bp-18h]@54
        
        int zone_id = usable_zone_ids[idx];
        zone_1 = this->zones[zone_id];
        if (worldContainsZoneId(zone_id) && (type_1 != ZONETYPE_Goal || puzzles_can_be_reused <= 0) )
            continue;
        
        switch (type_1) {
            case ZONETYPE_Empty:
                if ( field_2E64 ) {
                    count_1 = (int)zone_1->_hotspots.size();
                    if ( count_1 <= 0 ) {
                        v42 = -1;
                        return zone_id;
                    }
                    int idx_1 = 0;
                    int hotspot_idx = 0;
                    do {
                        if ( (zone_1->_hotspots[hotspot_idx])->type == 13 )
                            break;
                        ++hotspot_idx;
                        ++idx_1;
                    }
                    while ( idx_1 < count_1 );
                } else if ( zone_1->getType() == ZONETYPE_Empty ) {
                    v42 = -1;
                    return zone_id;
                }
                continue;
            case ZONETYPE_BlockadeNorth:
                if ( zone_1->getType() != ZONETYPE_BlockadeNorth
                    || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                v42 = -1;
                return zone_id;
            case ZONETYPE_BlockadeSouth:
                if ( zone_1->getType() != ZONETYPE_BlockadeSouth
                    || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 ) {
                    continue;
                }
                v42 = -1;
                return zone_id;
            case ZONETYPE_BlockadeEast:
                if ( zone_1->getType() != ZONETYPE_BlockadeEast || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                v42 = -1;
                return zone_id;
            case ZONETYPE_BlockadeWest:
                if ( zone_1->getType() != ZONETYPE_BlockadeWest || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                v42 = -1;
                return zone_id;
            case ZONETYPE_TravelStart:
                if ( zone_1->getType() != ZONETYPE_TravelStart || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                v42 = -1;
                return zone_id;
            case ZONETYPE_TravelEnd:
                if ( zone_1->getType() != ZONETYPE_TravelEnd || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                    continue;
                v42 = -1;
                return zone_id;
            case ZONETYPE_Goal:
                if ( zone_1->getType() != ZONETYPE_Goal )
                    continue;
                if ( ZoneLeadsToItem(zone_id, item_id_1) != 1 )
                    continue;
                if ( ZoneLeadsToItem(zone_id, item_id_2) != 1 )
                    continue;
                
                item_id[0] = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                v20 = GetItemIDThatsNotRequiredYet(zone_id, a4, 1);
                item_ids = v20;
                if ( item_id[0] < 0 || (signed int)v20 < 0 )
                    continue;
                v21 = GetNewPuzzleId(item_id[0], item_id_1, ZONETYPE_Goal, !a3);
                v22 = v21;
                if ( v21 >= 0 )
                    this->puzzle_ids.push_back(v21);
                puzzle_id = GetNewPuzzleId((__int16)item_ids, item_id_2, ZONETYPE_Goal, !a3);
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
                if ( Unknown_7(zone_id, a3, a4, item_id_3, a8) != 1 ) {
                    this->puzzle_ids_1[v33 / 2] = -1;
                    this->puzzle_ids_2[v24] = -1;
                    continue;
                }
                AddRequiredQuestWithItemID(item_id[0], item_id_3);
                AddRequiredQuestWithItemID((__int16)item_ids, item_id_3);
                v42 = -1;
                return zone_id;
            case ZONETYPE_Town:
                if ( zone_1->getType() != 11 ) continue;
                v42 = -1;
                return zone_id;
            case ZONETYPE_Trade:
                if ( zone_1->getType() != 15 ) continue;
                if ( ZoneLeadsToItem(zone_id, item_id_1) != 1 ) continue;
                v38 = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( v38 < 0 ) continue;
                v25 = GetNewPuzzleId(v38, item_id_1, ZONETYPE_Trade, !a3);
                v26 = v25;
                if ( v25 < 0 ) continue;
                
                if(a8) this->puzzle_ids_1[a3] = v25;
                else  this->puzzle_ids_2[a3] = v25;
                
                if ( Unknown_1(zone_id, a3, item_id_3, a8) != 1 ) continue;
                this->puzzle_ids.push_back(v26);
                AddRequiredQuestWithItemID(v38, item_id_3);
                v42 = -1;
                return zone_id;
            case ZONETYPE_Use:
            {
                if ( zone_1->getType() != 16 )
                    continue;
                if ( ZoneLeadsToItem(zone_id, item_id_1) != 1 )
                    continue;
                int16 puzzleID1 = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
                if ( puzzleID1 < 0 ) continue;
                
                int16 puzzleID2 = GetNewPuzzleId(puzzleID1, item_id_1, ZONETYPE_Use, !a3);
                if ( puzzleID2 < 0 ) continue;
                
                if(a8) this->puzzle_ids_1[a3] = puzzleID2;
                else this->puzzle_ids_2[a3] = puzzleID2;
                
                if ( use_ids_from_array_1(zone_id, a3, item_id_3, a8) != 1 ) continue;
                this->puzzle_ids.push_back(puzzleID2);
                
                AddRequiredQuestWithItemID(puzzleID1, item_id_3);
                v42 = -1;
                return zone_id;
            }
            case ZONETYPE_Find:
                zone_type = zone_1->getType();
                if ( (zone_type != ZONETYPE_Find && zone_type != ZONETYPE_FindTheForce)
                    || AssociateItemWithZoneHotspot(zone_id, item_id_1, item_id_3) != 1 )
                {
                    continue;
                }
                v42 = -1;
                return zone_id;
            default:
                continue;
        }
    }
    
    return -1;
}


signed int YodaDocument::Unknown_1(int16 zone_id, int16 a3, int16 zone_index, int16 a8) {
    if(false) return 0x10;
    if(false) return 0x10;
    
    if(false == 0xF) return 0x10;
    if(false > 0) return 0x10;
    
    AddRequiredQuestWithItemID(0, 0);
    AddRequiredQuestWithItemID(0, 0);
    
    if(!ZoneDoesNOTProvideRequiredItemID(0)) {
        RemoveQuestRequiringItem(0);
        RemoveQuestRequiringItem(0);
        
        return 0x10;
    }
    
    if(!ChooseItemIDFromZone_1(0, 0, 0, 0, 0)) {
        // Unknown_14()
    }
    
    AddRequiredItemsFromHotspots(0);
    
    return 0;
}
void YodaDocument::RemoveEmptyZoneIdsFromWorld(){}
int YodaDocument::Unknown_5(int16* world){
    printf("YodaDocument::Unknown_5(...)\n");
    
    int v3; // edi@1
    int v4; // eax@1
    Quest *quest; // ebp@2
    __int16 v6; // bx@2
    uint16 zone_id_2; // ax@3
    int word_idy; // ebx@4
    char *v9; // edx@4
    int v10; // ebx@6
    int v11; // edi@6
    Quest *v12; // ecx@7
    int intermediate_puzzle_item; // edi@12
    int distance; // eax@15
    __int16 v15; // bx@15
    signed int result; // eax@18
    int zone_id; // edi@22
    Zone *zone; // ecx@27
    int hotspot_count; // eax@28
    WorldSize world_size; // eax@35
    int v22; // edx@57
    char *v23; // eax@57
    int v24; // ST1C_4@62
    int v25; // ST18_4@62
    __int16 v26; // ax@62
    int v27; // ST18_4@62
    int zone_id_3; // eax@62
    Zone *zone_1; // ecx@63
    int idx; // ebp@63
    char *v31; // edx@63
    int v33; // [sp+8h] [bp-28h]@0
    int v34; // [sp+Ch] [bp-24h]@0
    int x; // [sp+10h] [bp-20h]@2
    int y; // [sp+14h] [bp-1Ch]@2
    int zone_id_1; // [sp+18h] [bp-18h]@10
    int v38; // [sp+1Ch] [bp-14h]@1
    int v39; // [sp+20h] [bp-10h]@39
    int has_hotspot_that_can_provide_item; // [sp+24h] [bp-Ch]@28
    int v41; // [sp+2Ch] [bp-4h]@1
    
    this->AddProvidedQuestWithItemID(THE_FORCE, 2);
    this->AddProvidedQuestWithItemID(LOCATOR, 1);
    
    v3 = 0;
    v4 = (int)this->providedItems.size();
    v41 = 0;
    v38 = v4;
    
   
    if ( this->providedItems.size() <= 0 ) {
    LABEL_5:
        printf("YodaDocument::Unknown_5 -> cleanup\n");
        if ( v38 > 0 ) {
            v10 = 0;
            v11 = v38;
            do
            {
                v12 = this->providedItems[v10];
                if ( v12 )
                    ; // dealloc v12
                
                ++v10;
                v11--;
            }
            while ( v11 );
        }
        this->providedItems.clear();
        this->item_ids.clear();
        
        v38 = 0;
        zone_id_1 = 0;
        y = 0;
        do
        {
            x = 0;
            do
            {
                intermediate_puzzle_item = world[x + 10 * y];
                if ( intermediate_puzzle_item == 1 || intermediate_puzzle_item == 300 || intermediate_puzzle_item == 104 )
                {
                    distance = Map::GetDistanceToCenter(x, y);
                    v15 = distance;
                    this->field_2E64 = intermediate_puzzle_item == 104 || distance < 2;
                    if ( this->field_2E64 )
                    {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    }
                    else if ( false ) // used to be x
                    {
                        zone_id = zone_id_1;
                    }
                    else
                    {
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, distance, 0);
                    }
                    if ( zone_id < 0 )
                        return 0;
                    while ( 1 )
                    {
                        zone = this->zones[zone_id];
                        if ( this->field_2E64 )
                            break;
                        has_hotspot_that_can_provide_item = 0;
                        hotspot_count = (int)zone->_hotspots.size();
                        if ( hotspot_count > 0 )
                        {
                            int idx = 0;
                            do
                            {
                                if ( (zone->_hotspots[idx])->type == 13 )
                                    has_hotspot_that_can_provide_item = 1;
                                ++idx;
                                --hotspot_count;
                            }
                            while ( hotspot_count );
                        }
                        if ( !has_hotspot_that_can_provide_item )
                            break;
                        if ( !y )
                        {
                            y = 1;
                            v39 = v33;
                            v38 = v34;
                            if ( !x )
                                break;
                        LABEL_56:
                            x = 0;
                            zone_id_1 = -1;
                            break;
                        }
                        world_size = this->size;
                        if ( world_size == WORLD_SIZE_SMALL )
                        {
                            if ( abs((char *)v39 - (char *)v33) > 1 || abs((char *)v38 - (char *)v34) > 1 )
                            {
                                ++y;
                                v39 = v33;
                                v38 = v34;
                                if ( !x )
                                    break;
                                goto LABEL_56;
                            }
                        }
                        else if ( world_size == WORLD_SIZE_MEDIUM )
                        {
                            if ( abs((char *)v39 - (char *)v33) > 1 || abs((char *)v38 - (char *)v34) > 1 )
                            {
                                ++y;
                                v39 = v33;
                                v38 = v34;
                                if ( !x )
                                    break;
                                goto LABEL_56;
                            }
                        }
                        else
                        {
                            if ( world_size != WORLD_SIZE_LARGE )
                                break;
                            if ( abs((char *)v39 - (char *)v33) > 2 || abs((char *)v38 - (char *)v34) > 2 )
                            {
                                ++y;
                                v39 = v33;
                                v38 = v34;
                                if ( !x )
                                    break;
                                goto LABEL_56;
                            }
                        }
                        zone_id_1 = zone_id;
                        x = 1;
                        zone_id = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, v15, 0);
                        if ( zone_id < 0 )
                            return 0;
                    }
//                    v22 = (int)v33 + 10 * (_DWORD)v34;
                    v23 = (char *)this + 52 * v22;
  //                  *((_DWORD *)v23 + 302) = 1;
                    this->worldZones[v22] = zone;
                    *((int *)v23 + 602) = zone_id;
                    *((int *)v23 + 608) = -1;
                    *((int *)v23 + 606) = -1;
                    *((int *)v23 + 610) = -1;
                    *((int *)v23 + 612) = -1;
                    this->AddZoneWithIdToWorld(zone_id);
                    if ( zone_id == zone_id_1 )
                    {
                        zone_id_1 = -1;
                        x = 0;
                    }
                }
                x++;
            }
            while ( x < 10 );
            y++;
        }
        while ( y < 10 );
        
        if ( y == 1 )
        {
            v24 = v38;
            v25 = v39;
            this->field_2E64 = 1;
            v26 = Map::GetDistanceToCenter(v25, v24);
            zone_id_3 = this->GetZoneIdWithType(ZONETYPE_Empty, -1, -1, -1, -1, v26, v27);
            if ( (int16)zone_id_3 >= 0 )
            {
                zone_1 = this->zones[zone_id_3];
                idx = zone_id_1 + 10 * y;
                this->world_things[idx].zone_type = (ZONE_TYPE)1;
                // v31 = (char *)this + 52 * idx;
                this->worldZones[idx] = zone_1;
                /*
                *((int *)v31 + 602) = zone_id_3;
                *((int *)v31 + 608) = -1;
                *((int *)v31 + 606) = -1;
                *((int *)v31 + 610) = -1;
                *((int *)v31 + 612) = -1;
                 */
                this->AddZoneWithIdToWorld(zone_id_3);
            }
        }
        result = 1;
    }
    else
    {
        while ( 1 )
        {
            int x = 0, y = 0;
            printf("v3 = %d\n", v3);
            printf("v3 = %d\n", v3);
            quest = this->providedItems[v3];
            if ( place_puzzles__(quest->unknown, world, &x, &y) != 1 )
                break;
            zone_id_2 = this->GetZoneIdWithType(ZONETYPE_Find, -1, -1, quest->itemID, -1, quest->unknown, 0);
            if ( zone_id_2 < 0 )
                return 0;
            
            ++v3;
            word_idy = x + 10 * y;
/*            v9 = (char *)document + offsetof(YodaDocument, field_34) * word_idy;
            *((_DWORD *)v9 + 302) = 17;
            *((int *)v9 + 610) = this->wg_last_added_item_id?;*/
            this->worldZones[word_idy] = this->zones[(int16)zone_id_2];
//            *((int *)v9 + 602) = zone_id_2;
         
            world[word_idy] = 306;
          
            this->AddZoneWithIdToWorld(zone_id_2);
            if ( ++v41 >= v38 ) {
                goto LABEL_5;
            }
        }
        result = 0;
    }
    
    return result;
}

int YodaDocument::place_puzzles__(int a1 /*maxDistance*/, int16 *a2, int* a3, int* a4) {
    printf("YodaDocument::place_puzzles__(%d, .., .., ..)\n", a1);
    // a3 = xref
    // a4 = yref
    // a2 = world
    int v5; // esi@1
    int v6; // ebx@2
    MapPoint* v8; // eax@6
    MapPoint *v9; // eax@28
    signed int v10; // esi@34
    bool v11; // zf@34
    bool v12; // sf@34
    signed int v13; // esi@37
    int v14; // edx@37
    vector<MapPoint*>*v15; // eax@37
    signed int v17; // esi@42
    MapPoint* v18; // eax@44
    int v19; // edi@46
    int v20; // esi@46
    int v21; // edi@49
    int v22; // esi@49
    int v23; // edi@52
    int v24; // esi@52
    vector<MapPoint*> v27; // [sp+14h] [bp-64h]@1
    vector<MapPoint*> v30; // [sp+28h] [bp-50h]@1
    vector<MapPoint*> v33; // [sp+3Ch] [bp-3Ch]@1
   
    int v41; // [sp+64h] [bp-14h]@1
    void *v42; // [sp+68h] [bp-10h]@6
    int v43; // [sp+74h] [bp-4h]@1
    
    v5 = 0;
    v43 = 0;
    v43 = 1;
    v41 = 0;
    v43 = 2;
    
    for(int v5=0; v5 < 10; v5++) {
        v6 = 0;
        for(int v6=0; v6 < 10; v6++) {
            int idx = v6 + 10 * v5;
            int item = a2[idx];
            if ( Map::GetDistanceToCenter(v6, v5) > a1 )  {
                if ( item == 1 ) {
                    v9 = new MapPoint(v6, v5);
                    v42 = v9;
                    v43 = 4;
                    v43 = 2;
                    v27.push_back(v9);
                }
            }
            else if ( item == 1 || item == 300 )
            {
                v8 = new MapPoint(v6, v5);
                v42 = v8;
                v43 = 3;
                
                v43 = 2;
                
                if (( v6 < 1 || a2[idx-1] != 306 )
                    && ( v6 > 8 || a2[idx+1] != 306 )
                    && ( v5 < 1 || a2[idx-10] != 306 )
                    && ( v5 > 8 || a2[idx+10] != 306 ))
                    v30.push_back(v8);
                else
                    v33.push_back(v8);
            }
        }
    }

    v10 = (int)v30.size();
    v11 = v30.size() == 0;
    v12 = 0;
    v15 = &v30;
    if ( !v30.size() )
    {
        if ( !v33.size() )
        {
            if ( v27.size() )
            {
                v13 = (int)v27.size();
                v14 = win_rand() % v13;
                v15 = &v27;
                goto LABEL_44;
            }
            printf("No Place to put puzzle!\n");
            v41 = 0;
        }
        v11 = v10 == 0;
        v12 = v10 < 0;
    }
    
    if ( v12 || v11 )
    {
        v17 = (int)v33.size();
        if ( v33.size() <= 0 )
            goto LABEL_45;
        v14 = win_rand() % v17;
        v15 = &v33;
    }
    else
    {
        v14 = win_rand() % v10;
        v15 = &v30;
    }
LABEL_44:
    v18 = (*v15)[v14];
    v41 = 1;
    *a3 = v18->x;
    *a4 = v18->y;
    printf("YodaDocument::place_puzzles__: %dx%d\n", v18->x, v18->y);
LABEL_45:
    if ( v30.size() > 0 )
    {
        v19 = 0;
        v20 = (int)v30.size();
        do
        {
            v19 += 4;
            // TODO: clear point
            ; // operator delete(*(void **)(v31 + v19 - 4));
            --v20;
        }
        while ( v20 );
    }
    v30.clear();
    if ( v33.size() > 0 )
    {
        v21 = 0;
        v22 = (int)v33.size();
        do
        {
            // TODO: clear point
            v21 += 4;
            // operator delete(*(void **)(&v33[0] + v21 - 4));
            --v22;
        }
        while ( v22 );
    }
    v33.clear();
    if ( v27.size() > 0 )
    {
        v23 = 0;
        v24 = (int)v27.size();
        do
        {
            v23 += 4;
            // TODO: clear point
            // operator delete(*(void **)(&v27[0] + v23 - 4));
            --v24;
        }
        while ( v24 );
    }
    v27.clear();
    v43 = 1;
    v43 = 0;
    v43 = -1;
    return v41;
}