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
typedef enum {
    WorldSize_SMALL = 1,
    WorldSize_MEDIUM = 2,
    WorldSize_LARGE = 3
} WorldSize;

class YodaDocument {
public:
    Planet planet;
    WorldSize size;
    __uint16_t seed;
    
    vector<Puzzle*> puzzles;
    vector<uint16> chosen_puzzle_ids;
    
    int puzzles_can_be_reused;
    
    YodaDocument();
    
    int16 GetNewPuzzleId(__uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5);
    void GetPuzzleCandidates(vector<int16> &result, __uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5);
    int ContainsPuzzleId(uint16 puzzle_id);
    int PuzzleUsedInLastGame(uint16 puzzle_id, Planet lastPlanet);
    int PuzzleIsGoal(uint16 puzzle_id, Planet planet);
    void ShuffleVector(vector<int16> &result);
};

#endif /* YodaDocument_hpp */
