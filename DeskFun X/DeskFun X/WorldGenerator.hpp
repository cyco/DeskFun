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
};

#endif /* WorldGenerator_hpp */
