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
    bool generateWorld(uint16 seed, WorldSize size);
    
private:
    int doLoop0(YodaDocument *doc, const int puzzle_count, const int puzzles2_count, const uint16* puzzles);
    ZONE_TYPE zoneTypeForWorldItem(MapType item);
    int doPuzzle(YodaDocument *doc, int x, int y, MapType zone_2, int* did_not_place_zone);
    int doCleanup(YodaDocument* doc);
    int loop1(YodaDocument* doc, int puzzleMapIdx, uint16* puzzles, int v199, int x_8);
    int loop2(YodaDocument* doc, uint16* map);
    int placeTransport(YodaDocument *doc, uint16* map);
};

#endif /* WorldGenerator_hpp */
