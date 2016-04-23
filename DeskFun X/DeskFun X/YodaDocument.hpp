//
//  YodaDocument.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef YodaDocument_hpp
#define YodaDocument_hpp
#include "CppTypes.h"
#include <stdio.h>
#include <vector>
#include "Puzzle.hpp"
#include "types.h"
#include "Zone.hpp"
#include "WorldThing.hpp"
#include "Quest.hpp"
#include "GameDataParser.hpp"

typedef enum {
    WorldSize_SMALL = 1,
    WorldSize_MEDIUM = 2,
    WorldSize_LARGE = 3
} WorldSize;

class YodaDocument {
public:
    Planet planet;
    WorldSize size;
    uint16 seed;
    
    int field_68;
    int wg_npc_id; // originally field_3390
    int wg_item_id, wg_last_added_item_id, field_2E64, field_3394, field_3398, wg_item_id_unknown_3, wg_item_id_unknown_2, goal_puzzle_id_again, goal_tile_id_1, goal_tile_id_2, wg_zone_type;
    vector<int> some_array;
    
    vector<WorldThing> world_things;
    vector<Zone*> zones;
    vector<Tile*> tiles;
    vector<Puzzle*> puzzles;
    vector<uint16> puzzle_ids;
    vector<uint16> chosen_zone_ids;
    vector<Zone*> chosen_zones;
    
    vector<uint16> puzzle_ids_1;
    vector<uint16> puzzle_ids_2;
    vector<uint16> item_ids;
    
    vector<Quest*> providedItems;
    vector<Quest*> requiredItems;
    
    vector<Zone*> worldZones;
    
    int logging;
    
    vector<uint16> tatooine_puzzle_ids;
    vector<uint16> hoth_puzzle_ids;
    vector<uint16> endor_puzzle_ids;
    
    int puzzles_can_be_reused;
    
    YodaDocument(const char* path);
    GameDataParser *parser;
    
    int16 GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5);
    void GetPuzzleCandidates(vector<int16> &result, uint16 item_id, int a3, ZONE_TYPE zone_type, int a5);
    int ContainsPuzzleId(uint16 puzzle_id);
    int PuzzleUsedInLastGame(uint16 puzzle_id, Planet lastPlanet);
    int PuzzleIsGoal(uint16 puzzle_id, Planet planet);
    void ShuffleVector(vector<int16> &result);
#pragma mark -
    Zone* getZoneByID(const uint16 zoneID);
    int worldContainsZoneId(uint16 zoneID);
    void addZoneWithIdToWorld(uint16 zoneID);
    uint16 getZoneID(const Zone *zone);
    uint16 getZoneIDAt(const int x, const int y);
    Quest* AddProvidedQuestWithItemID(const uint16 itemID, const uint16 unknown);
    Quest* AddRequiredQuestWithItemID(const uint16 itemID, const uint16 unknown);
    void RemoveQuestProvidingItem(uint16 itemID);
    void RemoveQuestRequiringItem(uint16 itemID);
    int HasQuestRequiringItem(uint16 itemID);
    int getLocationOfZoneWithID(uint16 zoneID, int *xOut, int *yOut);
    void addRequiredItemsFromHotspots(uint16 zoneID);
    bool ZoneHasProvidedItem(uint16 zoneID, uint16 itemID);
    int GetItemIDThatsNotRequiredYet(uint16 zone_id, int unused, bool use_array_2_ids);
    signed int GenerateWorld(int seed, int puzzle_count, int16* map, int16 *puzzleMap);
    int SetupRequiredItemForZone_(int16 zone_id, int16 arg2, int use_required_items_array);
    signed int RequiredItemForZoneWasNotPlaced(const uint16 zone_id);
    int16 GetZoneIdWithType(ZONE_TYPE type_1, int a3, int a4, int item_id_1, int item_id_2, int16 item_id_3, int a8);
    int AssociateItemWithZoneHotspot(int16 zone_id, int item_id, int a4);
    int Unknown_7(int16 zone_id, int16 puzzle_idx, int16 a4, int unknown, int a6);
    signed int ChooseItemIDFromZone(uint16 zoneID, uint16 itemID, int a4);
    signed int ChooseItemIDFromZone_0(int16 zone_id, int item_id);
    signed int ChooseItemIDFromZone_1(int16 a2, int a3, int a4, int16 a5, int a6);
    bool ZoneHasItem(uint16 zone_id, uint16 itemID, int a4);
    signed int use_ids_from_array_1(int16 zone_id, int16 a3, int16 item_id_1, int16 a5);
    signed int ChooseSpawnForPuzzleNPC(int16 a2, int a3);
    int16 findUnusedNPCForZone(uint16 zone_id);
    bool hasPuzzleNPC(uint16 zoneID, int16 targetNPCID);
    signed int Unknown_1(int16 zone_id, int16 a3, int16 zone_index, int16 a8);
    
    void RemoveEmptyZoneIdsFromWorld();
    int Unknown_5(int16* unknown);
    int place_puzzles__(int maxDistance, int16* world, int* xref, int* yref);
    void GetTileProvidedByZonesHotspots(int16 zone_id);
    int Unknown_14(int16 zoneID, int16 a3, uint16 distance, uint16 providedItemID);
    void WritePlanetValues();
};

#endif /* YodaDocument_hpp */
