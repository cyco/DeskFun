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
#endif /* Quest_hpp */
