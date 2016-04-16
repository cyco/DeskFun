//
//  Quest.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 05/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef Quest_hpp
#define Quest_hpp

#include <stdio.h>
#include "CppTypes.h"
class Quest {
public:
    uint16 itemID;
    uint16 unknown;
    Quest(uint16 itemID, uint16 unknown){
        this->itemID = itemID;
        this->unknown = unknown;
    }
};

class MapPoint {
public:
    int x;
    int y;
    
    MapPoint(int x, int y){
        this->x = x;
        this->y = y;
    }
};
#endif /* Quest_hpp */
