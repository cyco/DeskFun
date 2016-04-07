//
//  YodaDocument.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "YodaDocument.hpp"

YodaDocument::YodaDocument(){
    puzzles_can_be_reused = -1;
}

void YodaDocument::ShuffleVector(vector<int16> &array)
{
    vector<int16> temp_array;
    
    int16 count = array.size();
    temp_array.resize(count, -1);
    if ( count <= 0 ) return;
    
    for(int i=0; i < count; i++) {
        int random = win_rand();
        int idx = random % count;
        if ( temp_array[idx] == -1 ) {
            temp_array[idx] = array[i];
            array[i] = -1;
        }
    }
    
    int16 v18 = count - 1;
    if ( v18 >= 0 ) {
        do {
            if (array[v18] != -1 ) {
                int did_find_free_spot = 0;
                int16 v17 = 0;
                do {
                    for(int i=0; i < count; i++) {
                        if ( temp_array[i] == -1 )
                            did_find_free_spot = 1;
                    }
                    
                    if ( !did_find_free_spot ) break;
                    
                    int random = win_rand();
                    int idx = random % count;
                    
                    if ( temp_array[idx] == -1 ) {
                        ++v17;
                        temp_array[idx] = array[v18];
                        array[v18] = -1;
                    }
                }
                while ( !v17 );
            }
            --v18;
        }
        while ( v18 >= 0 );
    }
    
    for(int i=0; i < count; i++) {
        array[i] = temp_array[i];
    }
}

int16 YodaDocument::GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    int16 puzzle_id; // dx@57
    Puzzle *puzzle_1; // ebx@57
    int break_from_loop = 0;
    
    vector<int16> puzzle_ids;
    GetPuzzleCandidates(puzzle_ids, item_id, a3, zone_type, a5);
    if (puzzle_ids.size() <= 0 ) {
        return -1;
    }
    
    printf("Array::Shuffle (%lu items): ", puzzle_ids.size());
    for(int16 id : puzzle_ids) printf("0x%02x, ", id);
    printf("\n");
    ShuffleVector(puzzle_ids);
    printf("Array::Shuffle: (%lu items): ", puzzle_ids.size());
    for(int16 id : puzzle_ids) printf("0x%02x, ", id);
    printf("\n");
    
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
                    return puzzle_ids[puzzle_idx];
                }
            }
            else if ( zone_type == ZONETYPE_Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->questItemIDs[0] == item_id )
            {
                return puzzle_ids[puzzle_idx];
            }
            goto LABEL_75;
        }
        if ( zone_type != ZONETYPE_Use )
            break;
        if ( puzzle_1->type == PuzzleTypeU1 && puzzle_1->questItemIDs[0] == item_id )
        {
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
        return -1;
    }
    
    if ( zone_type != 9999 || puzzle_1->type != PuzzleTypeEnd )
        goto LABEL_75;
    
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
    for(uint16 id : chosen_puzzle_ids) {
        if(id == puzzle_id) return 1;
    }
    return 0;
}

int YodaDocument::PuzzleUsedInLastGame(uint16 puzzle_id, Planet planet) {
    return 0;
}

int YodaDocument::PuzzleIsGoal(uint16 puzzle_id, Planet planet){
    if ( planet == TATOOINE )
    {
        switch ( puzzle_id )
        {
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
    
    if ( planet == HOTH )
    {
        switch ( puzzle_id )
        {
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
            || puzzle_id == GOAL_LANTERN_OF_SACRED_LIGHT) )
    {
        return 1;
    }
    
    return 0;
}
#pragma mark -
Zone* YodaDocument::getZoneByID(uint16 zoneID) {
    return zones[zoneID];
}

int YodaDocument::worldContainsZoneId(uint16 zoneID) {
    for(uint16 chosenZoneID : chosen_zone_ids)
        if(chosenZoneID == zoneID) return 1;
    
    return 0;
}
void YodaDocument::AddZoneWithIdToWorld(uint16 zoneID){
    chosen_zone_ids.push_back(zoneID);
}

uint16 YodaDocument::getZoneID(Zone *zone){
    for(int i=0; i < zones.size(); i++)
        if(zones[i] == zone) return i;
    return -1;
}

uint16 YodaDocument::getZoneIDAt(int x, int y){
    return world_things[x + 10 * y].zone_id;
}

Quest* YodaDocument::AddProvidedQuestWithItemID(uint16 itemID, uint16 unknown){
    providedItems.push_back(new Quest(itemID, unknown));
    return providedItems[requiredItems.size()-1];
}

Quest* YodaDocument::AddRequiredQuestWithItemID(uint16 itemID, uint16 unknown){
    for(Quest *quest : requiredItems)
        if(quest->itemID == itemID) return quest;
    
    requiredItems.push_back(new Quest(itemID, unknown));
    return requiredItems[requiredItems.size()-1];
}

void YodaDocument::RemoveQuestProvidingItem(uint16 itemID) {
    for(int i=0; i < providedItems.size(); i++)
        if(providedItems[i]->itemID == itemID) {
            providedItems.erase(providedItems.begin()+i);
            return;
        }
}

void YodaDocument::RemoveQuestRequiringItem(uint16 itemID) {
    for(int i=0; i < requiredItems.size(); i++)
        if(requiredItems[i]->itemID == itemID) {
            requiredItems.erase(requiredItems.begin()+i);
            return;
        }
}

int YodaDocument::HasQuestRequiringItem(uint16 itemID) {
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

void YodaDocument::AddRequiredItemsFromHotspots(uint16 zoneID) {
    Zone* zone = zones[zoneID];
    for (Hotspot hotspot : zone->_hotspotss) {
        switch (hotspot.type) {
            case CrateItem:
            case PuzzleNPC:
            case CrateWeapon:
                if((hotspot.arg1 & 0x8000u) == 0)
                    AddRequiredQuestWithItemID(hotspot.arg1, -1);
                break;
            case DoorIn:
                if((hotspot.arg1 & 0x8000u) == 0)
                    AddRequiredItemsFromHotspots(hotspot.arg1);
                break;
            default:
                break;
        }
    }
}

int YodaDocument::ZoneLeadsToItem(uint16 zoneID, uint16 itemID) {
    Zone *zone = getZoneByID(zoneID);
    for(uint16 itemIDInZone : zone->providedItemIDs)
        if(itemIDInZone == itemID) return 1;
    
    for(Hotspot hotspot : zone->_hotspotss)
        if(hotspot.type == DoorIn && ZoneLeadsToItem(hotspot.arg1, itemID))
            return 1;
    
    return 0;
}


int YodaDocument::GetItemIDThatsNotRequiredYet(__int16 zone_id, int unused, int use_array_2_ids)
{
    vector<uint16> itemIDs;
    Zone *zone = zones[zone_id];
    
    vector<uint16>* zoneItemIds = use_array_2_ids ? &(zone->assignedItemIDs) : &(zone->requiredItemIDs);
    for(uint16 itemID : *zoneItemIds) {
        if(!HasQuestRequiringItem(itemID)) {
            itemIDs.push_back(itemID);
        }
    }

    if(!itemIDs.size()){
        return itemIDs[rand() % itemIDs.size()];
    }
    
    for(Hotspot hotspot : zone->_hotspotss){
        if(hotspot.type == DoorIn) {
            uint16 itemID = GetItemIDThatsNotRequiredYet(hotspot.arg1, unused, use_array_2_ids);
            if(itemID >= 0) return itemID;
        }
    }
    
    return -1;
}