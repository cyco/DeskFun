//
//  TestSuite.hpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 21/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#ifndef TestSuite_hpp
#define TestSuite_hpp

#include <stdio.h>
#include <dispatch/dispatch.h>
#include "MapGenerator.hpp"
#include "YodaDocument.hpp"

class TestSuite {
public:
    TestSuite();
    
    int runTests();
    int runMapTests();
    int runWorldTests();
    
private:
    
    dispatch_queue_t queue;
    int testMap(uint16 seed, WorldSize size, uint16 *expected_map, int16 *expected_puzzles);
    int testWorld(uint16 seed, WorldSize size, uint32 *expected_result);
};
#endif /* TestSuite_hpp */
