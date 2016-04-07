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

int TILE_SPEC_THE_FORCE  = 0x40;
int TILE_SPEC_USEFUL  = 0x80;
int TILE_SPEC_MAP    = 0x100000;

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
    for(uint16 id : puzzle_ids) {
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
    return providedItems[providedItems.size()-1];
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
    for (Hotspot hotspot : zone->_hotspots) {
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
    
    for(Hotspot hotspot : zone->_hotspots)
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
    
    for(Hotspot hotspot : zone->_hotspots){
        if(hotspot.type == DoorIn) {
            uint16 itemID = GetItemIDThatsNotRequiredYet(hotspot.arg1, unused, use_array_2_ids);
            if(itemID >= 0) return itemID;
        }
    }
    
    return -1;
}
#pragma mark -
signed int YodaDocument::GenerateWorld(int seed, int puzzle_count, int16* map, int16 *puzzleMap)
{
    return 0;
}


signed int YodaDocument::ZoneDoesNOTProvideRequiredItemID(__int16 zone_id)
{
    signed int result_1; // esi@1
    Zone *v3; // ebx@1
    signed int result; // eax@2
    int count_1; // ebp@3
    int hotspot_offset; // edi@4
    Hotspot *hotspot; // ecx@5
    unsigned int hotspot_type; // eax@5
    __int16 item_id; // ax@9
    __int16 item_id_1; // ax@12
    YodaDocument *document; // [sp+10h] [bp-8h]@1
    int count; // [sp+14h] [bp-4h]@3
    
    document = this;
    result_1 = 1;
    v3 = this->zones[zone_id];
    if ( v3 )
    {
        count_1 = 0;
        count = (int)v3->_hotspots.size();
        if ( count > 0 )
        {
            hotspot_offset = 0;
            do
            {
                hotspot = &v3->_hotspots[hotspot_offset];
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
        result = result_1;
    }
    else
    {
        result = 0;
    }
    return result;
}

signed int YodaDocument::SetupRequiredItemForZone_(__int16 zone_id, __int16 arg2, int use_required_items_array)
{
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
    YodaDocument *document; // [sp+24h] [bp-10h]@1
    int v22; // [sp+30h] [bp-4h]@1
    
    document = this;
    item_ids_1.clear();
    
    v22 = 0;
    if ( zone_id < 0 ){
        v22 = -1;
        return 0;
    }
    
    zone = document->zones[zone_id];
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
                if ( document->item_ids[documentItemIdx] == zone->providedItemIDs[idx])
                    v14 = 1;
                ++documentItemIdx;
                --count_1;
            }
            while ( count_1 );
        }
        if ( !v14 )
        {
            document->item_ids.push_back(zone->providedItemIDs[idx]);
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
    document->wg_item_id = random_item_id;
    AddRequiredItemsFromHotspots(zone_id);
    v22 = -1;
    
    return 1;
}

int YodaDocument::AssociateItemWithZoneHotspot(__int16 zone_id, int item_id, int a4)
{
    int v4; // ebp@0
    signed int found_item_id_in_zone_items; // esi@1
    Zone *zone; // edi@3
    int item_ids; // eax@6
    int v8; // ebx@6
    __int16 *item_id_ptr; // edx@7
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
    int v21; // [sp+10h] [bp-2Ch]@24
    int hotspot_indexes; // [sp+14h] [bp-28h]@21
    int item_id_1; // [sp+20h] [bp-1Ch]@13
    int v24; // [sp+24h] [bp-18h]@19
    int didFindSuitableHotspot; // [sp+28h] [bp-14h]@1
    YodaDocument *document; // [sp+2Ch] [bp-10h]@1
    int v27; // [sp+38h] [bp-4h]@13
    
    document = this;
    found_item_id_in_zone_items = 0;
    didFindSuitableHotspot = 0;
    if ( zone_id >= 0 )
    {
        if ( ZoneDoesNOTProvideRequiredItemID(zone_id) )
        {
            zone = document->zones[zone_id];
            if ( zone )
            {
                if ( zone->requiredItemIDs.size() <= 0 && zone->puzzleNPCTileIDs.size() <= 0 )
                {
                    item_ids = zone->providedItemIDs.size();
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
                                if ( zone->_hotspots[idx].type == hotspot_type )
                                {
                                    v20.push_back(idx);
                                    // CWordArray::SetAtGrow((WordArray *)&v20, hotspot_indexes, idx);
                                    didFindSuitableHotspot = 1;
                                }
                                ++idx;
                            }
                            while ( v24 > idx );
                        }
                        if ( didFindSuitableHotspot == 1 )
                        {
                            v13 = hotspot_indexes;
                            hotspot = &zone->_hotspots[*(int *)(v21 + 2 * (rand() % v13))];
                            hotspot->arg1 = item_id;
                            // hotspot->enabled = 1;
                            AddRequiredQuestWithItemID(item_id, a4);
                            wg_last_added_item_id = item_id_1;
                        }
                        v27 = -1;
                    }
                    else
                    {
                        idx_1 = 0;
                        v16 = 0;
                        v24 = zone->_hotspots.size();
                        if ( v24 > 0 )
                        {
                            do
                            {
                                hotspot_1 = &zone->_hotspots[idx_1];
                                if ( hotspot_1->type == DoorIn )
                                {
                                    zone_id_1 = hotspot_1->arg1;
                                    if ( zone_id_1 >= 0 )
                                        didFindSuitableHotspot = AssociateItemWithZoneHotspot(
                                                                                                            
                                                                                                            zone_id_1,
                                                                                                            item_id,
                                                                                                            a4);
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
    int v8; // ebp@0
    int zone_count; // esi@1
    YodaDocument *document; // edi@1
    int zone_index; // ebx@3
    Zone *zone; // ecx@4
    int type; // eax@9
    Zone **zones; // ecx@17
    Zone *zone_1; // esi@17
    int count_1; // edx@24
    int idx_1; // ecx@25
    Hotspot **hotspot; // eax@25
    __int16 v20; // eax@50
    int v21; // eax@52
    int v22; // esi@52
    int puzzle_id; // eax@54
    int v24; // esi@58
    int v25; // eax@65
    int16 v26; // si@65
    __int16 *v27; // ecx@67
    int v28; // eax@74
    int16 v29; // si@74
    __int16 *v30; // ecx@76
    int zone_type; // eax@80
    vector<int16> usable_zone_ids; // [sp+Ch] [bp-40h]@3
    unsigned int v33; // [sp+20h] [bp-2Ch]@58
    int break_1; // [sp+24h] [bp-28h]@1
    __int16 item_id[2]; // [sp+28h] [bp-24h]@50
    __int16 item_ids; // [sp+2Ch] [bp-20h]@50
    int a5a; // [sp+30h] [bp-1Ch]@1
    __int16 v38; // [sp+34h] [bp-18h]@54
    __int16 count; // [sp+3Ah] [bp-12h]@16
    __int16 idx; // [sp+3Ch] [bp-10h]@16
    int zone_id; // [sp+3Eh] [bp-Eh]@17
    int v42; // [sp+48h] [bp-4h]@3
    
    
    break_1 = 0;
    zone_count = this->zones.size();
    document = this;
    a5a = 0;
    if ( !a3 ) a5a = 1;
    zone_index = 0;

    usable_zone_ids.clear();
    v42 = 0;
    if ( zone_count > 0 )
    {
        do
        {
            zone = document->zones[zone_index];
            if ( (size_t)zone != -1 && zone && zone->getPlanet() == document->planet )
            {
                switch ( type_1 )
                {
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
                            goto add_to_temp_array;
                        break;
                    case ZONETYPE_Find:
                        case ZONETYPE_FindTheForce: // ?
                        type = zone->getType();
                        if ( type == ZONETYPE_Find || type == ZONETYPE_FindTheForce )
                            add_to_temp_array:
                            usable_zone_ids.push_back(zone_index);
                        break;
                    default:
                        break;
                }
            }
            zone_index = zone_index + 1;
        }
        while (zone_index < zone_count );
    }
    
    if ( !usable_zone_ids.size() )
    {
    return_failure:
        v42 = -1;
        return -1;
    }
    ShuffleVector(usable_zone_ids);
    
    idx = 0;
    zone_index = item_id_3;
    count = usable_zone_ids.size();
    int zone_idx = 0;
    while ( 1 )                                   // find unused zone from candidates
    {
        zone_idx = 0;
        zone_id = usable_zone_ids[idx];
        zone_1 = zones[zone_id];
        if ( !worldContainsZoneId(zone_id) || (type_1 == ZONETYPE_Goal && puzzles_can_be_reused > 0) )
            break;
    return_failure_if_at_end:
        if ( ++idx >= count )
            ++break_1;
        if ( break_1 )
            goto return_failure;
    }
    
    switch (type_1 )
    {
        case ZONETYPE_Empty:
            if ( field_2E64 )
            {
                count_1 = zone_1->_hotspots.size();
                if ( count_1 <= 0 )
                {
                    v42 = -1;
                    return zone_id;
                }
                idx_1 = 0;
                int hotspot_idx;
                do
                {
                    if ( (zone->_hotspots[hotspot_idx]).type == 13 )
                        break;
                    ++hotspot_idx;
                    ++idx_1;
                }
                while ( idx_1 < count_1 );
            }
            else if ( zone_1->getType() == ZONETYPE_Empty )
            {
                v42 = -1;
                return zone_id;
            }
            goto return_failure_if_at_end;
        case ZONETYPE_BlockadeNorth:
            if ( zone_1->getType() != ZONETYPE_BlockadeNorth
                || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
            {
                goto return_failure_if_at_end;
            }
            v42 = -1;
            return zone_id;
        case ZONETYPE_BlockadeSouth:
            if ( zone_1->getType() != ZONETYPE_BlockadeSouth
                || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
            {
                goto return_failure_if_at_end;
            }
            v42 = -1;
            return zone_id;
        case ZONETYPE_BlockadeEast:
            if ( zone_1->getType() != ZONETYPE_BlockadeEast || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                goto return_failure_if_at_end;
            v42 = -1;
            return zone_id;
        case ZONETYPE_BlockadeWest:
            if ( zone_1->getType() != ZONETYPE_BlockadeWest || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                goto return_failure_if_at_end;
            v42 = -1;
            return zone_id;
        case ZONETYPE_TravelStart:
            if ( zone_1->getType() != ZONETYPE_TravelStart || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                goto return_failure_if_at_end;
            v42 = -1;
            return zone_id;
        case ZONETYPE_TravelEnd:
            if ( zone_1->getType() != ZONETYPE_TravelEnd || SetupRequiredItemForZone_(zone_id, item_id_3, 0) != 1 )
                goto return_failure_if_at_end;
            v42 = -1;
            return zone_id;
        case ZONETYPE_Goal:
            if ( zone_1->getType() != ZONETYPE_Goal )
                goto return_failure_if_at_end;
            if ( ZoneLeadsToItem(zone_id, item_id_1) != 1 )
                goto return_failure_if_at_end;
            if ( ZoneLeadsToItem(zone_id, item_id_2) != 1 )
                goto return_failure_if_at_end;
            
            item_id[0] = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
            v20 = GetItemIDThatsNotRequiredYet(zone_id, a4, 1);
            item_ids = v20;
            if ( item_id[0] < 0 || (signed int)v20 < 0 )
                goto return_failure_if_at_end;
            v21 = GetNewPuzzleId(item_id[0], item_id_1, ZONETYPE_Goal, a5a);
            v22 = v21;
            if ( v21 >= 0 )
                document->puzzle_ids.push_back(v21);
            puzzle_id = GetNewPuzzleId((__int16)item_ids, item_id_2, ZONETYPE_Goal, a5a);
            v38 = puzzle_id;
            if ( puzzle_id >= 0 )
            {
                document->puzzle_ids.push_back(puzzle_id);
            }
            if ( v22 < 0 || v38 < 0 )
                goto return_failure_if_at_end;
            
            document->puzzle_ids_1[(int16)a3] = v22;
            v33 = 2 * (int16)a3;
            v24 = (int16)a4;
            document->puzzle_ids_2[v24] = v38;
            if ( Unknown_7(zone_id, a3, a4, zone_index, a8) != 1 )
            {
                document->puzzle_ids_1[v33 / 2] = -1;
                document->puzzle_ids_2[v24] = -1;
                goto return_failure_if_at_end;
            }
            AddRequiredQuestWithItemID(item_id[0], item_id_3);
            AddRequiredQuestWithItemID((__int16)item_ids, item_id_3);
            v42 = -1;
            return zone_id;
        case ZONETYPE_Town:
            if ( zone_1->getType() != 11 )
                goto return_failure_if_at_end;
            v42 = -1;
            return zone_id;
        case ZONETYPE_Trade:
            if ( zone_1->getType() != 15 )
                goto return_failure_if_at_end;
            if ( YodaDocument::ZoneLeadsToItem(zone_id, item_id_1) != 1 )
                goto return_failure_if_at_end;
            v38 = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
            if ( v38 < 0 )
                goto return_failure_if_at_end;
            v25 = GetNewPuzzleId(v38, item_id_1, ZONETYPE_Trade, a5a);
            v26 = v25;
            if ( v25 < 0 )
                goto return_failure_if_at_end;
            
            if(a8){
                document->puzzle_ids_1[a3] = v25;
            } else {
                document->puzzle_ids_2[a3] = v25;
            }
            if ( sub_41F970(zone_id, a3, zone_index, a8) != 1 )
                goto return_failure_if_at_end;
            document->puzzle_ids.push_back(v26);
            AddRequiredQuestWithItemID(v38, item_id_3);
            v42 = -1;
            return zone_id;
        case ZONETYPE_Use:
            if ( zone_1->getType() != 16 )
                goto return_failure_if_at_end;
            if ( YodaDocument::ZoneLeadsToItem(zone_id, item_id_1) != 1 )
                goto return_failure_if_at_end;
            v38 = GetItemIDThatsNotRequiredYet(zone_id, a3, 0);
            if ( v38 < 0 )
                goto return_failure_if_at_end;
            v28 = GetNewPuzzleId(v38, item_id_1, ZONETYPE_Use, a5a);
            v29 = v28;
            if ( v28 < 0 )
                goto return_failure_if_at_end;
            if(a8){
                document->puzzle_ids_1[a3] = v28;
            } else {
                document->puzzle_ids_2[a3] = v28;
            }

            if ( use_ids_from_array_1(zone_id, a3, item_id_3, a8) != 1 )
                goto return_failure_if_at_end;
            document->puzzle_ids.push_back(v29);
            AddRequiredQuestWithItemID(v38, item_id_3);
            v42 = -1;
            return zone_id;
        case ZONETYPE_Find:
            zone_type = zone_1->getType();
            if ( zone_type != ZONETYPE_Find && zone_type != ZONETYPE_FindTheForce
                || AssociateItemWithZoneHotspot(zone_id, item_id_1, zone_index) != 1 )
            {
                goto return_failure_if_at_end;
            }
            v42 = -1;
            return zone_id;
        default:
            goto return_failure_if_at_end;
    }
 //*/
    return -1;
 }
