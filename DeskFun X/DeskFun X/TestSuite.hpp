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
    typedef struct {
        uint16 seed;
        uint16 size;
        uint16 planet;


        uint16 data[100];
    } MapSample;

    typedef struct {
        uint16 seed;
        uint16 size;
        uint16 planet;


        uint16 data[1000];
    } WorldSample;

public:
    TestSuite();
    
    int runTests();
    int runMapTests();
    int runWorldTests();
    int rumWorldTests();
    
private:
    dispatch_queue_t queue;
    int testMap(uint16 seed, WorldSize size, uint16 *expected_map, int16 *expected_puzzles);
    int testWorld(uint16 seed, WorldSize size, uint16 planet, uint16 *sample);
    int parseMapLine(FILE *mapFile, MapSample &sample);
    int parseWorldLine(FILE *mapFile, WorldSample &sample);
    uint16 parseToken(FILE* file, bool* stop);

};
#endif /* TestSuite_hpp */
