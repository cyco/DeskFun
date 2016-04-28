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
    
    vector<int> some_array;
    
    vector<Zone*> zones;
    vector<Tile*> tiles;
    vector<Puzzle*> puzzles;
    
    
    int logging;
    
    vector<uint16> tatooine_puzzle_ids;
    vector<uint16> hoth_puzzle_ids;
    vector<uint16> endor_puzzle_ids;
    
    int puzzles_can_be_reused;
    int goal_puzzle_id_again, goal_tile_id_1, goal_tile_id_2;

    
    YodaDocument(const char* path);
    ~YodaDocument();
    GameDataParser *parser;
#pragma mark -
};

#endif /* YodaDocument_hpp */
