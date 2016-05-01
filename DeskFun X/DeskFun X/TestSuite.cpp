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

//#define BASE_PATH "/Users/chris/Shared/Development/WebFun (github)/"
#define BASE_PATH "/Volumes/ramdisk/"
#define GAMEDATA   BASE_PATH "dist/rsrc/yoda.data"
#define FIXTURE_PATH_WORLDS BASE_PATH "test/fixtures/worlds.txt"
#define FIXTURE_PATH_TYPEMAPS BASE_PATH "test/fixtures/type_maps.txt"
#define FIXTURE_PATH_ORDERMAPS BASE_PATH "test/fixtures/order_maps.txt"

TestSuite::TestSuite(){
    queue = dispatch_queue_create("tests", DISPATCH_QUEUE_SERIAL);
}

int TestSuite::runTests() {
    int success = 1;

    // success &= runMapTests();
    success &= rumWorldTests();

    dispatch_barrier_sync(queue, ^{
        printf("all tests run\n");
    });

    return success;
}

int TestSuite::parseMapLine(FILE *mapFile, MapSample &sample){
    bool stop = false;
    sample.seed = parseToken(mapFile, &stop);
    if(stop) return false;
    sample.planet = parseToken(mapFile, &stop);
    if(stop) return false;
    sample.size = parseToken(mapFile, &stop);
    if(stop) return false;

    for(int i=0; i < 100; i++) {
        sample.data[i] = parseToken(mapFile, &stop);
        if(stop) return false;
    }
    return true;
}

int TestSuite::parseWorldLine(FILE *mapFile, WorldSample &sample){
    bool stop = false;
    sample.seed = parseToken(mapFile, &stop);
    if(stop) return false;
    sample.planet = parseToken(mapFile, &stop);
    if(stop) return false;
    sample.size = parseToken(mapFile, &stop);
    if(stop) return false;

    for(int i=0; i < 1000; i++) {
        sample.data[i] = parseToken(mapFile, &stop);
        if(stop) return false;
    }
    return true;
}


uint16 TestSuite::parseToken(FILE* file, bool *stop){
    char buffer[100];
    int zeroRunLength = 0;

    bool numberStarted = false;
    int idx = 0;
    do {
        if(idx >= 99) {
            *stop = true;
            return -1;
        }

        if(!fread(buffer+idx, 1, 1, file)) {
            *stop = true;
            return -1;
        }

        char c = buffer[idx];
        bool isHexDigit = ('0' <= c && c <= '9') || ('a' <= c  && c <= 'f');

        if(!isHexDigit) {
            if(numberStarted) break;
            else continue;
        }

        if(c == '0' && !numberStarted) {
            zeroRunLength ++;
            if(zeroRunLength != 4)
                continue;
        }

        idx ++;
        numberStarted = true;
    } while(true);


    if(!numberStarted) {
        *stop = true;
        return -1;
    }

    buffer[idx] = '\0';
    *stop = false;

    uint16 result = (uint16)strtol(buffer, NULL, 0x10);
    return result;
}

int TestSuite::runMapTests() {
    int success = 1;

    FILE *typeMapFile = fopen(FIXTURE_PATH_TYPEMAPS, "r");
    FILE *orderMapFile = fopen(FIXTURE_PATH_ORDERMAPS, "r");

    fseek(typeMapFile, 0x20, SEEK_CUR);
    fseek(orderMapFile, 0x20, SEEK_CUR);

    int i=0;
    bool stop = false;
    do {
        MapSample typeSample, orderSample;
        stop = !parseMapLine(typeMapFile, typeSample);
        if(stop) { return 0; }
        stop = !parseMapLine(orderMapFile, orderSample);
        if(stop) { printf("-= UNEXPECTED EOF =-\n"); return 0; }

        if(typeSample.seed != orderSample.seed) {
            printf("Seed mismatch\n");
            return 0;

        }
        if(typeSample.size != orderSample.size) {
            printf("Size mismatch\n");
            return 0;
        }
        if(typeSample.planet != orderSample.planet) {
            printf("Planet mismatch\n");
            return 0;
        }

        i++;
        dispatch_async(queue, ^{
            testMap(typeSample.seed, (WorldSize)typeSample.size, (uint16*)typeSample.data, (int16*)orderSample.data);
        });
    } while(!stop);

    return 0;
    return success;
}

int TestSuite::rumWorldTests() {
    int success = 1;

    FILE *worldFile = fopen(FIXTURE_PATH_WORLDS, "r");
    fseek(worldFile, 0x1c, SEEK_CUR);

    int i=0;
    bool stop = false;
    do {
        WorldSample worldSample;
        stop = !parseWorldLine(worldFile, worldSample);
        if(stop) { return 0; }

        i++;

        if(worldSample.seed != 0x047d ||
           worldSample.size != 2 ||
           worldSample.planet != 2) continue;

        /*
         [FAIL] 0x047d medium Hoth
         [FAIL] 0x0a85 medium Tatooine
         [FAIL] 0x0c08 large Hoth
         [FAIL] 0x10aa small Tatooine
         [FAIL] 0x21ee medium Tatooine
         [FAIL] 0x2270 small Tatooine
         [FAIL] 0x2c13 medium Tatooine
         [FAIL] 0x3c99 small Hoth
         [FAIL] 0x40e9 small Endor
         [FAIL] 0x456b small Endor
         [FAIL] 0x5707 small Tatooine
         [FAIL] 0x5a6e large Hoth
         [FAIL] 0x5ee3 small Tatooine
         [FAIL] 0x6b82 medium Endor
         [FAIL] 0x7af5 medium Endor
         */
        /*
        if(worldSample.seed == 0x0a85 && worldSample.size == 2 && worldSample.planet == 1) continue;
        if(worldSample.seed == 0x21ee && worldSample.size == 2 && worldSample.planet == 1) continue;
        if(worldSample.seed == 0x2270 && worldSample.size == 1 && worldSample.planet == 1) continue;
        if(worldSample.seed == 0x29fe && worldSample.size == 1 && worldSample.planet == 3) continue;
        if(worldSample.seed == 0x2c13 && worldSample.size == 2 && worldSample.planet == 1) continue;

        if(worldSample.seed == 0x047d && worldSample.size == 2 && worldSample.planet == 2) continue;
        if(worldSample.seed == 0x0c08 && worldSample.size == 3 && worldSample.planet == 2) continue;
        if(worldSample.seed == 0x10aa && worldSample.size == 1 && worldSample.planet == 1) continue;
        if(worldSample.seed == 0x3c99 && worldSample.size == 1 && worldSample.planet == 2) continue;
        if(worldSample.seed == 0x40e9 && worldSample.size == 1 && worldSample.planet == 3) continue;
        if(worldSample.seed == 0x456b && worldSample.size == 1 && worldSample.planet == 3) continue;
        if(worldSample.seed == 0x7af5 && worldSample.size == 2 && worldSample.planet == 3) continue;
*/

        dispatch_async(queue, ^{
            testWorld(worldSample.seed, (WorldSize)worldSample.size, worldSample.planet, (uint16*)worldSample.data);
        });
    } while(!stop);
    
    return 0;
    return success;

    return 1;
}

int TestSuite::testMap(uint16 seed, WorldSize size, uint16 *expected_map, int16 *expected_puzzles) {
    int success = 1;

    DeleteLog();
    Message("Generate New Map (c++, 0x%x, 0x%x)\n", seed, size);
    win_srand(seed);

    /*
    FILE *sam = fopen("/Users/chris/Desktop/map_win.txt", "w+");
    for(int y=0; y < 10; y++){
        for(int x=0; x < 10; x++){
            int idx = x + y * 10;
            fprintf(sam, "%4d ", (int16)expected_map[idx]);
        }
        fprintf(sam, "\n");
    }
    fclose(sam);
    //*/

    MapGenerator generator(size);
    generator.generate();

    for(int i=0; i < 100; i++) {
        if(generator.map[i] != expected_map[i] || generator.puzzles[i] != expected_puzzles[i]) {
            success = 0;
            break;
        }
    }

    if(success) printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    else {
        printf("[FAIL] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    }

    if(!success) {
        FILE *real = fopen("/Users/chris/Desktop/map_c++.txt", "w+");
        FILE *sam = fopen("/Users/chris/Desktop/map_win.txt", "w+");

        for(int y=0; y < 10; y++){
            for(int x=0; x < 10; x++){
                int idx = x + y * 10;
                fprintf(sam,  "%4d ", (int16)expected_map[idx]);
                fprintf(real, "%4d ", (int16)generator.map[idx]);
            }

            fprintf(real, "\n");
            fprintf(sam, "\n");
        }

        fclose(real);
        fclose(sam);
    }

    return success;
}

int TestSuite::testWorld(uint16 seed, WorldSize size, uint16 planet, uint16 *sample) {
    int success = 1;
    DeleteLog();
    Message("Generate New Map (c++, 0x%x, 0x%x)\n", seed, size);

    YodaDocument *doc = new YodaDocument(GAMEDATA);
    doc->logging = 1;

    WorldGenerator generator(doc);
    generator.generateWorld(seed, (WorldSize)size, (Planet)planet);

    for(int i=0; i < 100; i++) {
        if(!(generator.worldThings[i].zone_id == sample[i*10]) ||
           !(generator.worldThings[i].zone_type == sample[i*10+1]) ||
           // !(generator.worldThings[i].findItemID == sample[i*10+6]) ||
           // !(generator.worldThings[i].requiredItemID == sample[i*10+4]) ||
           // !(generator.worldThings[i].unknown606 == sample[i*10+5]) ||
           // !(generator.worldThings[i].unknown612 == sample[i*10+7])
           false
           ) {
            success = 0;
            break;
        }
    }

    if(!success) {
        FILE *real = fopen("/Users/chris/Desktop/world_c++.txt", "w+");
        FILE *sam = fopen("/Users/chris/Desktop/world_win.txt", "w+");

        for(int y=0; y < 10; y++){
            for(int x=0; x < 10; x++){
                int idx = x + y * 10;
                fprintf(sam, "%4d ", (int16)sample[idx*10+0]);
                fprintf(real, "%4d ", (int16)generator.worldThings[idx].zone_id);
            }

            fprintf(real, "\n");
            fprintf(sam, "\n");
        }

        fclose(real);
        fclose(sam);
    }

    if(success) printf("[OK] 0x%04x %s %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"), planet == 1 ? "Tatooine" : (planet == 2 ? "Hoth" : "Endor"));
    else printf("[FAIL] 0x%04x %s %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"), planet == 1 ? "Tatooine" : (planet == 2 ? "Hoth" : "Endor"));

    delete doc;

    return success;

}
