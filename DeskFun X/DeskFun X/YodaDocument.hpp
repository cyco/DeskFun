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
    
    vector<Puzzle*> _puzzles;
    
    int GetNewPuzzleId(__uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5);
};

#endif /* YodaDocument_hpp */
