//
//  TestSuite.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 21/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "TestSuite.hpp"
#include "win_rand.h"
#include "MapGenerator.hpp"
#include "WorldGenerator.hpp"

#include "samples.h"
#include "complete_samples.h"

#define DATA_PATH "/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/yodesk.dta"

#define TestMap(_seed_, _size_) success &= testMap(_seed_, (WorldSize)_size_, world_ ## _seed_ ## _ ## _size_, puzzles_ ## _seed_ ## _ ## _size_)
#define xTestMap(_seed_, _size_) do {} while(false);

#define TestWorld(_seed_, _size_) success &= testWorld(_seed_, (WorldSize)_size_, world_things_ ## _seed_ ## _ ## _size_);
#define xTestWorld(_seed_, _size_) do {} while(false);

TestSuite::TestSuite(){
    queue = dispatch_queue_create("tests", DISPATCH_QUEUE_CONCURRENT);
}

int TestSuite::runTests() {
    int success = 1;
    
    success &= runMapTests();
    success &= runWorldTests();
    
    //*/
    /* VALID INVALIDS
     [self testSample:world_things_FCA5_3 seed:0xFCA5 size:WorldSize_LARGE];
     [self testSample:world_things_dead_3 seed:0xDEAD size:WorldSize_LARGE];
     //*/
    /* Failing in map generation
     [self testSample:world_things_568C_3 seed:0x568C size:WorldSize_LARGE];
     //*/

    return success;
}


int TestSuite::runMapTests() {
    int success = 1;
    
    TestMap(0xbeef, 2);
    TestMap(0x0008, 2);
    TestMap(0xcafe, 3);
    TestMap(0xdeed, 3);
    TestMap(0xdead, 3);
    TestMap(0xbeef, 3);
    TestMap(0xffff, 3);
    TestMap(0x0123, 3);
    TestMap(0x1234, 3);
    TestMap(0x4321, 3);
    TestMap(0x0321, 3);
    TestMap(0x0000, 3);
    TestMap(0x1111, 3);
    TestMap(0x2222, 3);
    TestMap(0x3333, 3);
    TestMap(0x4444, 3);
    TestMap(0x5555, 3);
    TestMap(0x6666, 3);
    TestMap(0x8888, 3);
    TestMap(0x9999, 3);
    TestMap(0xaaaa, 3);
    TestMap(0xbbbb, 3);
    TestMap(0xdddd, 3);
    TestMap(0xeeee, 3);
    TestMap(0x0001, 3);
    TestMap(0x0002, 3);
    TestMap(0x0003, 3);
    TestMap(0x0004, 3);
    TestMap(0x0005, 3);
    TestMap(0x0006, 3);
    TestMap(0x0007, 3);
    TestMap(0x0009, 3);
    TestMap(0x000a, 3);
    TestMap(0x000c, 3);
    TestMap(0x000d, 3);
    TestMap(0x000f, 3);
    TestMap(0x000e, 3);
    TestMap(0x0008, 3);
    TestMap(0xcccc, 3);
    TestMap(0x7777, 3);
    TestMap(0x000b, 3);
    TestMap(0xdeed, 2);
    TestMap(0xdead, 2);
    TestMap(0xcafe, 2);
    TestMap(0xffff, 2);
    TestMap(0x0123, 2);
    TestMap(0x1234, 2);
    TestMap(0x4321, 2);
    TestMap(0x0321, 2);
    TestMap(0x0000, 2);
    TestMap(0x1111, 2);
    TestMap(0x2222, 2);
    TestMap(0x3333, 2);
    TestMap(0x4444, 2);
    TestMap(0x6666, 2);
    TestMap(0x7777, 2);
    TestMap(0x8888, 2);
    TestMap(0x9999, 2);
    TestMap(0xbbbb, 2);
    TestMap(0xcccc, 2);
    TestMap(0xdddd, 2);
    TestMap(0xeeee, 2);
    TestMap(0x0002, 2);
    TestMap(0x0004, 2);
    TestMap(0x0005, 2);
    TestMap(0x0006, 2);
    TestMap(0x0007, 2);
    TestMap(0x0009, 2);
    TestMap(0x000a, 2);
    TestMap(0x000b, 2);
    TestMap(0x000e, 2);
    TestMap(0x000f, 2);
    TestMap(0x5555, 2);
    TestMap(0xaaaa, 2);
    TestMap(0x0001, 2);
    TestMap(0x0003, 2);
    TestMap(0x000c, 2);
    TestMap(0x000d, 2);
    TestMap(0xdeed, 1);
    TestMap(0xdead, 1);
    TestMap(0xbeef, 1);
    TestMap(0xcafe, 1);
    TestMap(0xffff, 1);
    TestMap(0x0123, 1);
    TestMap(0x1234, 1);
    TestMap(0x4321, 1);
    TestMap(0x0321, 1);
    TestMap(0x0000, 1);
    TestMap(0x1111, 1);
    TestMap(0x2222, 1);
    TestMap(0x3333, 1);
    TestMap(0x4444, 1);
    TestMap(0x5555, 1);
    TestMap(0x6666, 1);
    TestMap(0x7777, 1);
    TestMap(0x8888, 1);
    TestMap(0x9999, 1);
    TestMap(0xaaaa, 1);
    TestMap(0xbbbb, 1);
    TestMap(0xcccc, 1);
    TestMap(0xdddd, 1);
    TestMap(0xeeee, 1);
    TestMap(0x0001, 1);
    TestMap(0x0002, 1);
    TestMap(0x0003, 1);
    TestMap(0x0004, 1);
    TestMap(0x0005, 1);
    TestMap(0x0006, 1);
    TestMap(0x0007, 1);
    TestMap(0x000b, 1);
    TestMap(0x000c, 1);
    TestMap(0x000d, 1);
    TestMap(0x000e, 1);
    TestMap(0x000f, 1);
    TestMap(0x000a, 1);
    TestMap(0x0009, 1);
    TestMap(0x0008, 1);
    
    dispatch_barrier_sync(queue, ^{
        printf("all done\n");
    });
    
    return success;
}

int TestSuite::runWorldTests() {
    int success = 1;
    
    TestWorld(0xaea6, 1);
    TestWorld(0xaea6, 2);
    TestWorld(0xaea6, 3);
    TestWorld(0x42b7, 1);
    TestWorld(0x42b7, 2);
    TestWorld(0x42b7, 3);
    TestWorld(0xc698, 1);
    TestWorld(0x1421, 1);
    TestWorld(0xebe9, 3);
    TestWorld(0xe43f, 3);
    TestWorld(0x1e77, 3);
    TestWorld(0x70A3, 3);
    TestWorld(0xdead, 1);
    TestWorld(0x08FF, 3);
    TestWorld(0xBB41, 3);
    TestWorld(0x60B2, 3);
    TestWorld(0x23C6, 3);

    return success;
}

int TestSuite::testMap(uint16 seed, WorldSize size, uint16 *expected_map, int16 *expected_puzzles) {
    int success = 1;
    
    win_srand(seed);
    
    MapGenerator generator(size);
    generator.generate();
    
    
    for(int i=0; i < 100; i++) {
        if(generator.map[i] != expected_map[i] || generator.puzzles[i] != expected_puzzles[i]) {
            success = 0;
            break;
        }
    }
    
    if(success) ; // printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    else printf("[FAIL] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    
    return success;
}

int TestSuite::testWorld(uint16 seed, WorldSize size, uint32 *sample) {
    int success = 1;
   
    YodaDocument *doc = new YodaDocument(DATA_PATH);
    doc->logging = 0;
    
    WorldGenerator generator(doc);
    generator.generateWorld(seed, (WorldSize)size);

    for(int i=0; i < 100; i++) {
        if(!(doc->world_things[i].zone_type == sample[i*4+0])
           || !(doc->world_things[i].zone_id == sample[i*4+1])
           || !(doc->world_things[i].findItemID == sample[i*4+2])
           || !(doc->world_things[i].requiredItemID == sample[i*4+3])) {
            success = 0;
            break;
        }
    }
    
    if(success) ; // printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    else printf("[FAIL] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));

    return success;
}
