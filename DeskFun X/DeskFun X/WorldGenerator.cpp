//
//  WorldGenerator.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 20/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "WorldGenerator.hpp"
WorldGenerator::WorldGenerator(YodaDocument *document) {
    doc = document;
}

bool WorldGenerator::generateRandomWorld(WorldSize size) {
    return this->generateWorld(win_rand(), size);
}

bool WorldGenerator::generateWorld(uint16 seed, WorldSize size) {
    return 0;
}