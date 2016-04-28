//
//  WorldGenerator.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 20/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef WorldGenerator_hpp
#define WorldGenerator_hpp

#include <stdio.h>
#include "YodaDocument.hpp"
#include "MapGenerator.hpp"

class WorldGenerator {
private:
    YodaDocument *doc;
    MapGenerator *mapGenerator;
    
public:
    WorldGenerator(YodaDocument *document);
    bool generateRandomWorld(WorldSize size);
    bool generateWorld(uint16 seed, WorldSize size, Planet planet);
    
private:
    int doLoop0(YodaDocument *doc, const int puzzle_count, const int puzzles2_count, const uint16* puzzles);
    ZONE_TYPE zoneTypeForWorldItem(MapType item);
    int doPuzzle(YodaDocument *doc, int x, int y, MapType zone_2, int* did_not_place_zone);
    int doCleanup(YodaDocument* doc);
    int loop1(YodaDocument* doc, int puzzleMapIdx, uint16* puzzles, int v199, int x_8);
    int loop2(YodaDocument* doc, uint16* map);
    int placeTransport(YodaDocument *doc, uint16* map);
    void RemoveEmptyZoneIdsFromWorld();
    int16 GetNewPuzzleId(uint16 item_id, int a3, ZONE_TYPE zone_type, int a5);
    int16 GetZoneIdWithType(ZONE_TYPE type_1, int a3, int a4, int item_id_1, int item_id_2, int16 item_id_3, int a8);
    signed int Unknown_1(int16 zone_id, int16 a3, int16 zone_index, int16 a8);
    int Unknown_5(int16* unknown);
    int Unknown_14(int16 zoneID, int16 a3, uint16 distance, uint16 providedItemID);

};

#endif /* WorldGenerator_hpp */
