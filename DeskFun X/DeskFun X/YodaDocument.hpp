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
    
    vector<WorldThing> world_things;
    vector<Zone*> zones;
    vector<Puzzle*> puzzles;
    vector<uint16> chosen_puzzle_ids;
    vector<uint16> chosen_zone_ids;
    vector<Zone*> chosen_zones;
    
    vector<Quest*> providedItems;
    vector<Quest*> requiredItems;
    
    int puzzles_can_be_reused;
    
    YodaDocument();
    
    int16 GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5);
    void GetPuzzleCandidates(vector<int16> &result, uint16 item_id, int a3, ZONE_TYPE zone_type, int a5);
    int ContainsPuzzleId(uint16 puzzle_id);
    int PuzzleUsedInLastGame(uint16 puzzle_id, Planet lastPlanet);
    int PuzzleIsGoal(uint16 puzzle_id, Planet planet);
    void ShuffleVector(vector<int16> &result);
#pragma mark -
    Zone* getZoneByID(uint16 zoneID);
    int worldContainsZoneId(uint16 zoneID);
    void AddZoneWithIdToWorld(uint16 zoneID);
    uint16 getZoneID(Zone *zone);
    uint16 getZoneIDAt(int x, int y);
    Quest* AddProvidedQuestWithItemID(uint16 itemID, uint16 unknown);
    Quest* AddRequiredQuestWithItemID(uint16 itemID, uint16 unknown);
    void RemoveQuestProvidingItem(uint16 itemID);
    void RemoveQuestRequiringItem(uint16 itemID);
    int HasQuestRequiringItem(uint16 itemID);
    int GetLocationOfZoneWithID(uint16 zoneID, int *xOut, int *yOut);
    void AddRequiredItemsFromHotspots(uint16 zoneID);
    int ZoneLeadsToItem(uint16 zoneID, uint16 itemID);
    int GetItemIDThatsNotRequiredYet(__int16 zone_id, int unused, int use_array_2_ids);
};

#endif /* YodaDocument_hpp */
