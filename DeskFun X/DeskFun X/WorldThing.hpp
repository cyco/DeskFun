//
//  WorldThing.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 05/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef WorldThing_hpp
#define WorldThing_hpp

#include <stdio.h>
#include "CppTypes.h"

class WorldThing
{
public:
    uint16 zone_id;
    ZONE_TYPE zone_type;
    
    uint16 unknown606;
    uint16 unknown608;
    uint16 unknown610;
    uint16 unknown612;
};

#endif /* WorldThing_hpp */
