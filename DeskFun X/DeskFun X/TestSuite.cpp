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

#include "full_samples.c"

#define DATA_PATH "/Users/chris/Desktop/Debugging/ mixed/Yoda Stories/yodesk.dta"

#define TestMap(_seed_, _size_) dispatch_async(queue, ^{ testMap(_seed_, (WorldSize)_size_, (uint16*)map_types_ ## _seed_ ## _0x2_ ## _size_, (int16*)map_order_ ## _seed_ ## _0x2_ ## _size_); })
#define xTestMap(_seed_, _size_) do {} while(false);

#define TestWorld(_seed_, _size_) dispatch_async(queue, ^{ testWorld(_seed_, (WorldSize)_size_, world_things_ ## _seed_ ## _ ## _size_); })
#define xTestWorld(_seed_, _size_) do {} while(false);


#define TestComplete(_seed_, _size_, _planet_) dispatch_async(queue, ^{ testCompleteWorld((uint16)_seed_, (WorldSize)_planet_, (Planet)_size_, (uint16*)complete_ ## _seed_ ## _0x ## _size_ ## _ ## _planet_); })
#define xTestComplete(_seed_, _size_, _planet_) do {} while(false);

TestSuite::TestSuite(){
    queue = dispatch_queue_create("tests", DISPATCH_QUEUE_SERIAL);
}

int TestSuite::runTests() {
    int success = 1;

    success &= runMapTests();
    // success &= runCompleteTets();

    dispatch_barrier_sync(queue, ^{
        printf("all tests run\n");
    });

    return success;
}


int TestSuite::parseLine(FILE *mapFile, MapSample &sample){
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

    buffer[idx+1] = '\0';
    *stop = false;

    uint16 result = (uint16)strtol(buffer, NULL, 0x10);
    return result;
}

int TestSuite::runMapTests() {
    int success = 1;

    FILE *typeMapFile = fopen("/Users/chris/Shared/Development/WebFun (github)/test/fixtures/type_maps.txt", "r");
    FILE *orderMapFile = fopen("/Users/chris/Shared/Development/WebFun (github)/test/fixtures/order_maps.txt", "r");

    fseek(typeMapFile, 0x20, SEEK_CUR);
    fseek(orderMapFile, 0x20, SEEK_CUR);

    bool stop = false;
    do {
        MapSample typeSample, orderSample;
        stop = !parseLine(typeMapFile, typeSample);
        if(stop) { return 0; }
        stop = !parseLine(orderMapFile, orderSample);
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

        testMap(typeSample.seed, (WorldSize)typeSample.size, (uint16*)typeSample.data, (int16*)orderSample.data);
    } while(!stop);

    return 0;
    return success;
}

int TestSuite::runCompleteTets() {
    TestComplete(0x15dd, 2, 3);
    /*
     TestComplete(0xdead, 2, 3);
     TestComplete(0x12c9, 2, 1);
     TestComplete(0x0001, 2, 1);
     TestComplete(0x0008, 2, 1);
     TestComplete(0x0009, 2, 1);
     TestComplete(0x0065, 2, 1);
     TestComplete(0x01e0, 2, 1);
     TestComplete(0x0214, 2, 1);
     TestComplete(0x01f7, 2, 1);
     TestComplete(0x0469, 2, 1);
     TestComplete(0x04d9, 2, 1);
     TestComplete(0x0776, 2, 1);
     TestComplete(0x077f, 2, 1);
     TestComplete(0x07dc, 2, 1);
     TestComplete(0x1398, 2, 3);
     TestComplete(0x086e, 2, 1);
     TestComplete(0x08c5, 2, 1);
     TestComplete(0x09a7, 2, 1);
     TestComplete(0x0c1e, 2, 1);
     TestComplete(0x0cf6, 2, 1);
     TestComplete(0x0ea5, 2, 1);
     TestComplete(0x111a, 2, 1);
     TestComplete(0x1398, 2, 1);
     TestComplete(0x18bb, 2, 1);
     TestComplete(0x19aa, 2, 1);
     TestComplete(0x1a60, 2, 1);
     TestComplete(0x1ade, 2, 1);
     TestComplete(0x1afe, 2, 1);
     TestComplete(0x1b7b, 2, 1);
     TestComplete(0x1c18, 2, 1);
     TestComplete(0x1d61, 2, 1);
     TestComplete(0x1d9a, 2, 1);
     TestComplete(0x1ecc, 2, 1);
     TestComplete(0x20d4, 2, 1);
     TestComplete(0x22cb, 2, 1);
     TestComplete(0x25ff, 2, 1);
     TestComplete(0x000b, 2, 1);
     TestComplete(0x278e, 2, 1);
     */
    /*
     TestComplete(0x0001, 2, 1);
     /*
     TestComplete(0x0000, 2, 1);
     TestComplete(0x0006, 2, 1);
     TestComplete(0x000c, 2, 1);
     TestComplete(0x000d, 2, 1);
     TestComplete(0x000f, 2, 1);
     TestComplete(0x000e, 2, 1);
     TestComplete(0x00ef, 2, 1);
     TestComplete(0x0123, 2, 1);
     TestComplete(0x0119, 2, 1);
     TestComplete(0x01c9, 2, 1);
     TestComplete(0x014c, 2, 1);
     TestComplete(0x024e, 2, 1);
     TestComplete(0x0270, 2, 1);
     TestComplete(0x0266, 2, 1);
     TestComplete(0x0321, 2, 1);
     TestComplete(0x0426, 2, 1);
     TestComplete(0x0363, 2, 1);
     TestComplete(0x044e, 2, 1);
     TestComplete(0x049f, 2, 1);
     TestComplete(0x0522, 2, 1);
     TestComplete(0x05b6, 2, 1);
     TestComplete(0x0592, 2, 1);
     TestComplete(0x0641, 2, 1);
     TestComplete(0x06c5, 2, 1);
     TestComplete(0x072b, 2, 1);
     TestComplete(0x0747, 2, 1);
     TestComplete(0x0828, 2, 1);
     TestComplete(0x0769, 2, 1);
     TestComplete(0x08c6, 2, 1);
     TestComplete(0x08f0, 2, 1);
     TestComplete(0x097c, 2, 1);
     TestComplete(0x09dc, 2, 1);
     TestComplete(0x0a5a, 2, 1);
     TestComplete(0x0cc9, 2, 1);
     TestComplete(0x0ded, 2, 1);
     TestComplete(0x0df9, 2, 1);
     TestComplete(0x10cc, 2, 1);
     TestComplete(0x10ef, 2, 1);
     TestComplete(0x1111, 2, 1);
     TestComplete(0x1204, 2, 1);
     TestComplete(0x13d9, 2, 1);
     TestComplete(0x1266, 2, 1);
     TestComplete(0x153b, 2, 1);
     TestComplete(0x15af, 2, 1);
     TestComplete(0x15ca, 2, 1);
     TestComplete(0x15de, 2, 1);
     TestComplete(0x15dd, 2, 1);
     TestComplete(0x15e0, 2, 1);
     TestComplete(0x15e8, 2, 1);
     TestComplete(0x1727, 2, 1);
     TestComplete(0x1750, 2, 1);
     TestComplete(0x184e, 2, 1);
     TestComplete(0x1864, 2, 1);
     TestComplete(0x18a4, 2, 1);
     TestComplete(0x18f4, 2, 1);
     TestComplete(0x193f, 2, 1);
     TestComplete(0x199a, 2, 1);
     TestComplete(0x199d, 2, 1);
     TestComplete(0x19d8, 2, 1);
     TestComplete(0x1a19, 2, 1);
     TestComplete(0x1a30, 2, 1);
     TestComplete(0x1acb, 2, 1);
     TestComplete(0x1b00, 2, 1);
     TestComplete(0x1b5f, 2, 1);
     TestComplete(0x1b99, 2, 1);
     TestComplete(0x1bf4, 2, 1);
     TestComplete(0x1c08, 2, 1);
     TestComplete(0x1d67, 2, 1);
     TestComplete(0x1dcd, 2, 1);
     TestComplete(0x1df9, 2, 1);
     TestComplete(0x1e79, 2, 1);
     TestComplete(0x1f9a, 2, 1);
     TestComplete(0x1ee9, 2, 1);
     TestComplete(0x1f9c, 2, 1);
     TestComplete(0x1fc7, 2, 1);
     TestComplete(0x2062, 2, 1);
     TestComplete(0x2097, 2, 1);
     TestComplete(0x2139, 2, 1);
     TestComplete(0x2222, 2, 1);
     TestComplete(0x2224, 2, 1);
     TestComplete(0x2250, 2, 1);
     TestComplete(0x2297, 2, 1);
     TestComplete(0x22d8, 2, 1);
     TestComplete(0x237b, 2, 1);
     TestComplete(0x23ac, 2, 1);
     TestComplete(0x245f, 2, 1);
     TestComplete(0x2595, 2, 1);
     TestComplete(0x2623, 2, 1);
     TestComplete(0x25aa, 2, 1);
     TestComplete(0x2641, 2, 1);
     TestComplete(0x2646, 2, 1);
     TestComplete(0x26c1, 2, 1);
     TestComplete(0x27c4, 2, 1);
     TestComplete(0x000a, 2, 1);
     TestComplete(0x0000, 2, 1);
     xTestComplete(0x052b, 2, 1);
     xTestComplete(0x055c, 2, 1);
     xTestComplete(0x055d, 2, 1);
     TestComplete(0x0592, 2, 1);
     TestComplete(0x05b6, 2, 1);
     TestComplete(0x0641, 2, 1);
     TestComplete(0x27c4, 2, 1);
     TestComplete(0x2641, 2, 1);
     TestComplete(0x2646, 2, 1);
     TestComplete(0x26c1, 2, 1);
     TestComplete(0x2595, 2, 1);
     TestComplete(0x25aa, 2, 1);
     TestComplete(0x2623, 2, 1);
     TestComplete(0x0006, 2, 1);
     TestComplete(0x000c, 2, 1);
     TestComplete(0x000d, 2, 1);
     TestComplete(0x000e, 2, 1);
     TestComplete(0x000f, 2, 1);
     xTestComplete(0x0077, 2, 1);
     TestComplete(0x00ef, 2, 1);
     TestComplete(0x0119, 2, 1);
     TestComplete(0x0123, 2, 1);
     TestComplete(0x014c, 2, 1);
     xTestComplete(0x0162, 2, 1);
     xTestComplete(0x01a3, 2, 1);
     xTestComplete(0x01b3, 2, 1);
     TestComplete(0x01c9, 2, 1);
     TestComplete(0x024e, 2, 1);
     xTestComplete(0x024f, 2, 1);
     TestComplete(0x0266, 2, 1);
     TestComplete(0x0270, 2, 1);
     xTestComplete(0x02cf, 2, 1);
     TestComplete(0x0321, 2, 1);
     TestComplete(0x0363, 2, 1);
     TestComplete(0x0426, 2, 1);
     TestComplete(0x044e, 2, 1);
     TestComplete(0x049f, 2, 1);
     TestComplete(0x0522, 2, 1);
     TestComplete(0x245f, 2, 1);
     TestComplete(0x237b, 2, 1);
     TestComplete(0x23ac, 2, 1);
     TestComplete(0x2297, 2, 1);
     TestComplete(0x22d8, 2, 1);
     TestComplete(0x2250, 2, 1);
     TestComplete(0x2222, 2, 1);
     TestComplete(0x2224, 2, 1);
     TestComplete(0x2139, 2, 1);
     TestComplete(0x2062, 2, 1);
     TestComplete(0x2097, 2, 1);
     TestComplete(0x1fc7, 2, 1);
     TestComplete(0x1f9a, 2, 1);
     TestComplete(0x1f9c, 2, 1);
     TestComplete(0x1e79, 2, 1);
     TestComplete(0x1ee9, 2, 1);
     TestComplete(0x1df9, 2, 1);
     TestComplete(0x1dcd, 2, 1);
     TestComplete(0x1d67, 2, 1);
     TestComplete(0x1c08, 2, 1);
     TestComplete(0x217f, 2, 1);
     return 0;
     TestComplete(0x0476, 2, 1);
     TestComplete(0x0691, 2, 1);
     TestComplete(0x06c5, 2, 1);
     xTestComplete(0x06d5, 2, 1);
     TestComplete(0x06e8, 2, 1);
     xTestComplete(0x0707, 2, 1);
     TestComplete(0x072b, 2, 1);
     xTestComplete(0x0736, 2, 1);
     TestComplete(0x0747, 2, 1);
     TestComplete(0x0769, 2, 1);
     xTestComplete(0x0794, 2, 1);
     xTestComplete(0x07cf, 2, 1);
     TestComplete(0x07e4, 2, 1);
     xTestComplete(0x080c, 2, 1);
     TestComplete(0x0828, 2, 1);
     xTestComplete(0x0858, 2, 1);
     xTestComplete(0x087e, 2, 1);
     xTestComplete(0x0897, 2, 1);
     xTestComplete(0x08bc, 2, 1);
     xTestComplete(0x08c0, 2, 1);
     TestComplete(0x08c6, 2, 1);
     TestComplete(0x08f0, 2, 1);
     xTestComplete(0x08FF, 2, 1);
     xTestComplete(0x091b, 2, 1);
     TestComplete(0x097c, 2, 1);
     xTestComplete(0x0985, 2, 1);
     xTestComplete(0x098e, 2, 1);
     xTestComplete(0x09b7, 2, 1);
     TestComplete(0x09bd, 2, 1);
     TestComplete(0x09dc, 2, 1);
     TestComplete(0x0a00, 2, 1);
     xTestComplete(0x0a46, 2, 1);
     TestComplete(0x0a5a, 2, 1);
     xTestComplete(0x0a5e, 2, 1);
     xTestComplete(0x0abb, 2, 1);
     TestComplete(0x0acc, 2, 1);
     xTestComplete(0x0af4, 2, 1);
     xTestComplete(0x0b5f, 2, 1);
     xTestComplete(0x0b79, 2, 1);
     xTestComplete(0x0b88, 2, 1);
     xTestComplete(0x0bae, 2, 1);
     xTestComplete(0x0bb5, 2, 1);
     xTestComplete(0x0bc1, 2, 1);
     xTestComplete(0x0bcb, 2, 1);
     xTestComplete(0x0bcf, 2, 1);
     xTestComplete(0x0c3b, 2, 1);
     xTestComplete(0x0c40, 2, 1);
     xTestComplete(0x0c50, 2, 1);
     xTestComplete(0x0c59, 2, 1);
     xTestComplete(0x0c74, 2, 1);
     xTestComplete(0x0c86, 2, 1);
     TestComplete(0x0cc9, 2, 1);
     xTestComplete(0x0cd7, 2, 1);
     xTestComplete(0x0d0d, 2, 1);
     TestComplete(0x0d77, 2, 1);
     xTestComplete(0x0d78, 2, 1);
     TestComplete(0x0ded, 2, 1);
     TestComplete(0x0df9, 2, 1);
     xTestComplete(0x0e33, 2, 1);
     TestComplete(0x0e72, 2, 1);
     xTestComplete(0x0e99, 2, 1);
     xTestComplete(0x0eef, 2, 1);
     xTestComplete(0x0ef3, 2, 1);
     xTestComplete(0x0f1c, 2, 1);
     xTestComplete(0x0f3c, 2, 1);
     TestComplete(0x0f90, 2, 1);
     xTestComplete(0x0fc3, 2, 1);
     xTestComplete(0x1008, 2, 1);
     xTestComplete(0x105a, 2, 1);
     TestComplete(0x10cc, 2, 1);
     xTestComplete(0x10e9, 2, 1);
     TestComplete(0x10ef, 2, 1);
     TestComplete(0x1111, 2, 1);
     TestComplete(0x1142, 2, 1);
     xTestComplete(0x1157, 2, 1);
     xTestComplete(0x115c, 2, 1);
     TestComplete(0x11b8, 2, 1);
     xTestComplete(0x11e4, 2, 1);
     xTestComplete(0x11ef, 2, 1);
     TestComplete(0x1204, 2, 1);
     xTestComplete(0x1218, 2, 1);
     xTestComplete(0x1234, 2, 1);
     xTestComplete(0x124b, 2, 1);
     xTestComplete(0x124c, 2, 1);
     TestComplete(0x1266, 2, 1);
     xTestComplete(0x12c8, 2, 1);
     xTestComplete(0x1323, 2, 1);
     xTestComplete(0x138e, 2, 1);
     xTestComplete(0x139c, 2, 1);
     xTestComplete(0x13b9, 2, 1);
     TestComplete(0x13d9, 2, 1);
     xTestComplete(0x13ee, 2, 1);
     xTestComplete(0x141e, 2, 1);
     xTestComplete(0x1421, 2, 1);
     xTestComplete(0x14f4, 2, 1);
     TestComplete(0x153b, 2, 1);
     TestComplete(0x15af, 2, 1);
     xTestComplete(0x15c6, 2, 1);
     TestComplete(0x15ca, 2, 1);
     xTestComplete(0x15cb, 2, 1);
     TestComplete(0x15dd, 2, 1);
     TestComplete(0x15de, 2, 1);
     TestComplete(0x15e0, 2, 1);
     TestComplete(0x15e8, 2, 1);
     xTestComplete(0x162f, 2, 1);
     TestComplete(0x1659, 2, 1);
     TestComplete(0x166f, 2, 1);
     xTestComplete(0x16be, 2, 1);
     xTestComplete(0x16da, 2, 1);
     xTestComplete(0x16dd, 2, 1);
     xTestComplete(0x16f1, 2, 1);
     TestComplete(0x1727, 2, 1);
     xTestComplete(0x174e, 2, 1);
     TestComplete(0x1750, 2, 1);
     xTestComplete(0x1817, 2, 1);
     xTestComplete(0x1842, 2, 1);
     TestComplete(0x184e, 2, 1);
     TestComplete(0x1864, 2, 1);
     xTestComplete(0x1876, 2, 1);
     xTestComplete(0x1881, 2, 1);
     TestComplete(0x18a4, 2, 1);
     TestComplete(0x18f4, 2, 1);
     TestComplete(0x1924, 2, 1);
     TestComplete(0x193f, 2, 1);
     TestComplete(0x199a, 2, 1);
     TestComplete(0x199d, 2, 1);
     TestComplete(0x19d8, 2, 1);
     TestComplete(0x1a19, 2, 1);
     TestComplete(0x1a30, 2, 1);
     TestComplete(0x1a9b, 2, 1);
     TestComplete(0x1acb, 2, 1);
     TestComplete(0x1b00, 2, 1);
     TestComplete(0x1b21, 2, 1);
     TestComplete(0x1b5f, 2, 1);
     xTestComplete(0x1B8C, 2, 1);
     TestComplete(0x1b99, 2, 1);
     TestComplete(0x1bf4, 2, 1);
     TestComplete(0x1bf8, 2, 1);
     xTestComplete(0x1cad, 2, 1);
     xTestComplete(0x1cc1, 2, 1);
     xTestComplete(0x1ccb, 2, 1);
     xTestComplete(0x1d69, 2, 1);
     TestComplete(0x1dec, 2, 1);
     xTestComplete(0x1e27, 2, 1);
     xTestComplete(0x1e29, 2, 1);
     TestComplete(0x1e77, 2, 1);
     xTestComplete(0x1f05, 2, 1);
     xTestComplete(0x1fbe, 2, 1);
     xTestComplete(0x2028, 2, 1);
     TestComplete(0x2040, 2, 1);
     xTestComplete(0x2059, 2, 1);
     xTestComplete(0x20ad, 2, 1);
     TestComplete(0x2122, 2, 1);
     TestComplete(0x217c, 2, 1);
     TestComplete(0x222f, 2, 1);
     TestComplete(0x2275, 2, 1);
     TestComplete(0x2296, 2, 1);
     TestComplete(0x241c, 2, 1);
     xTestComplete(0x2476, 2, 1);
     xTestComplete(0x24b1, 2, 1);
     xTestComplete(0x24f3, 2, 1);
     xTestComplete(0x2504, 2, 1);
     xTestComplete(0x2545, 2, 1);
     TestComplete(0x2552, 2, 1);
     xTestComplete(0x2568, 2, 1);
     xTestComplete(0x2574, 2, 1);
     xTestComplete(0x2585, 2, 1);
     xTestComplete(0x2589, 2, 1);
     xTestComplete(0x25fd, 2, 1);
     xTestComplete(0x2708, 2, 1);
     TestComplete(0x270e, 2, 1);
     xTestComplete(0x2729, 2, 1);
     xTestComplete(0x2751, 2, 1);
     xTestComplete(0x2766, 2, 1);
     xTestComplete(0x2780, 2, 1);
     xTestComplete(0x2781, 2, 1);
     xTestComplete(0x2786, 2, 1);
     xTestComplete(0x278f, 2, 1);
     xTestComplete(0x2829, 2, 1);
     xTestComplete(0x2844, 2, 1);
     xTestComplete(0x285f, 2, 1);
     xTestComplete(0x2897, 2, 1);
     xTestComplete(0x28d2, 2, 1);
     xTestComplete(0x28e5, 2, 1);
     xTestComplete(0x2925, 2, 1);
     xTestComplete(0x2962, 2, 1);
     xTestComplete(0x2970, 2, 1);
     xTestComplete(0x29d3, 2, 1);
     xTestComplete(0x29f3, 2, 1);
     xTestComplete(0x29fb, 2, 1);
     TestComplete(0x0007, 2, 1);
     TestComplete(0x0002, 2, 1);
     TestComplete(0x0003, 2, 1);
     TestComplete(0x0004, 2, 1);
     TestComplete(0x0005, 2, 1);
     TestComplete(0x0026, 2, 1);
     TestComplete(0x00c2, 2, 1);
     TestComplete(0x0282, 2, 1);
     TestComplete(0x0348, 2, 1);
     //*/
    /*
     TestComplete(0x2a30, 2, 1);
     TestComplete(0x2a43, 2, 1);
     TestComplete(0x2a88, 2, 1);
     TestComplete(0x2a98, 2, 1);
     TestComplete(0x2aa0, 2, 1);
     TestComplete(0x2ac5, 2, 1);
     TestComplete(0x2b13, 2, 1);
     TestComplete(0x2b5d, 2, 1);
     TestComplete(0x2b77, 2, 1);
     TestComplete(0x2ba7, 2, 1);
     TestComplete(0x2bc3, 2, 1);
     TestComplete(0x2bd0, 2, 1);
     TestComplete(0x2c31, 2, 1);
     TestComplete(0x2c46, 2, 1);
     TestComplete(0x2c8a, 2, 1);
     TestComplete(0x2c95, 2, 1);
     TestComplete(0x2ca6, 2, 1);
     TestComplete(0x2cb5, 2, 1);
     TestComplete(0x2cbb, 2, 1);
     TestComplete(0x2cbc, 2, 1);
     TestComplete(0x2ccb, 2, 1);
     TestComplete(0x2ce6, 2, 1);
     TestComplete(0x2d36, 2, 1);
     TestComplete(0x2d81, 2, 1);
     TestComplete(0x2d85, 2, 1);
     TestComplete(0x2d93, 2, 1);
     TestComplete(0x2d9c, 2, 1);
     TestComplete(0x2da0, 2, 1);
     TestComplete(0x2df3, 2, 1);
     TestComplete(0x2dfe, 2, 1);
     TestComplete(0x2e00, 2, 1);
     TestComplete(0x2e10, 2, 1);
     TestComplete(0x2e15, 2, 1);
     TestComplete(0x2e1a, 2, 1);
     TestComplete(0x2e2b, 2, 1);
     TestComplete(0x2e41, 2, 1);
     TestComplete(0x2e51, 2, 1);
     TestComplete(0x2e61, 2, 1);
     TestComplete(0x2e87, 2, 1);
     TestComplete(0x2ebe, 2, 1);
     TestComplete(0x2efd, 2, 1);
     TestComplete(0x2f1e, 2, 1);
     TestComplete(0x2f4c, 2, 1);
     TestComplete(0x2f51, 2, 1);
     TestComplete(0x2f52, 2, 1);
     TestComplete(0x2f70, 2, 1);
     TestComplete(0x2f73, 2, 1);
     TestComplete(0x2f7c, 2, 1);
     TestComplete(0x2f9f, 2, 1);
     TestComplete(0x2fb4, 2, 1);
     TestComplete(0x2fcc, 2, 1);
     TestComplete(0x2fea, 2, 1);
     TestComplete(0x2ff7, 2, 1);
     TestComplete(0x300b, 2, 1);
     TestComplete(0x3029, 2, 1);
     TestComplete(0x302b, 2, 1);
     TestComplete(0x3033, 2, 1);
     TestComplete(0x3040, 2, 1);
     TestComplete(0x305f, 2, 1);
     TestComplete(0x309e, 2, 1);
     TestComplete(0x30a8, 2, 1);
     TestComplete(0x30c9, 2, 1);
     TestComplete(0x30e6, 2, 1);
     TestComplete(0x30e9, 2, 1);
     TestComplete(0x3121, 2, 1);
     TestComplete(0x3149, 2, 1);
     TestComplete(0x3193, 2, 1);
     TestComplete(0x319a, 2, 1);
     TestComplete(0x319c, 2, 1);
     TestComplete(0x31c7, 2, 1);
     TestComplete(0x31d7, 2, 1);
     TestComplete(0x31dd, 2, 1);
     TestComplete(0x31e1, 2, 1);
     TestComplete(0x31ed, 2, 1);
     TestComplete(0x31fd, 2, 1);
     TestComplete(0x320e, 2, 1);
     TestComplete(0x321e, 2, 1);
     TestComplete(0x3248, 2, 1);
     TestComplete(0x3250, 2, 1);
     TestComplete(0x32ac, 2, 1);
     TestComplete(0x32db, 2, 1);
     TestComplete(0x330b, 2, 1);
     TestComplete(0x331b, 2, 1);
     TestComplete(0x3333, 2, 1);
     TestComplete(0x333e, 2, 1);
     TestComplete(0x3395, 2, 1);
     TestComplete(0x339d, 2, 1);
     TestComplete(0x33ca, 2, 1);
     TestComplete(0x3401, 2, 1);
     TestComplete(0x346b, 2, 1);
     TestComplete(0x3490, 2, 1);
     TestComplete(0x34a1, 2, 1);
     TestComplete(0x34a3, 2, 1);
     TestComplete(0x34c1, 2, 1);
     TestComplete(0x34c2, 2, 1);
     TestComplete(0x3509, 2, 1);
     TestComplete(0x3587, 2, 1);
     TestComplete(0x35b0, 2, 1);
     TestComplete(0x35d1, 2, 1);
     TestComplete(0x35d3, 2, 1);
     TestComplete(0x35e7, 2, 1);
     TestComplete(0x3600, 2, 1);
     TestComplete(0x3611, 2, 1);
     TestComplete(0x361e, 2, 1);
     TestComplete(0x3625, 2, 1);
     TestComplete(0x3638, 2, 1);
     TestComplete(0x3689, 2, 1);
     TestComplete(0x368a, 2, 1);
     TestComplete(0x36a3, 2, 1);
     TestComplete(0x36c5, 2, 1);
     TestComplete(0x36da, 2, 1);
     TestComplete(0x36fd, 2, 1);
     TestComplete(0x3711, 2, 1);
     TestComplete(0x371a, 2, 1);
     TestComplete(0x3738, 2, 1);
     TestComplete(0x375c, 2, 1);
     TestComplete(0x3765, 2, 1);
     TestComplete(0x37b1, 2, 1);
     TestComplete(0x37b7, 2, 1);
     TestComplete(0x381e, 2, 1);
     TestComplete(0x383b, 2, 1);
     TestComplete(0x385b, 2, 1);
     TestComplete(0x387a, 2, 1);
     TestComplete(0x3884, 2, 1);
     TestComplete(0x38c4, 2, 1);
     TestComplete(0x38f7, 2, 1);
     TestComplete(0x391b, 2, 1);
     TestComplete(0x391d, 2, 1);
     TestComplete(0x393a, 2, 1);
     TestComplete(0x3958, 2, 1);
     TestComplete(0x3989, 2, 1);
     TestComplete(0x39a3, 2, 1);
     TestComplete(0x39d8, 2, 1);
     TestComplete(0x3a0c, 2, 1);
     TestComplete(0x3a4a, 2, 1);
     TestComplete(0x3a8c, 2, 1);
     TestComplete(0x3abc, 2, 1);
     TestComplete(0x3ac6, 2, 1);
     TestComplete(0x3ae5, 2, 1);
     TestComplete(0x3ae8, 2, 1);
     TestComplete(0x3af3, 2, 1);
     TestComplete(0x3b09, 2, 1);
     TestComplete(0x3b4e, 2, 1);
     TestComplete(0x3b6d, 2, 1);
     TestComplete(0x3b9c, 2, 1);
     TestComplete(0x3be0, 2, 1);
     TestComplete(0x3c07, 2, 1);
     TestComplete(0x3c15, 2, 1);
     TestComplete(0x3c59, 2, 1);
     TestComplete(0x3c8f, 2, 1);
     TestComplete(0x3ca7, 2, 1);
     TestComplete(0x3ca9, 2, 1);
     TestComplete(0x3cb1, 2, 1);
     TestComplete(0x3cfc, 2, 1);
     TestComplete(0x3d03, 2, 1);
     TestComplete(0x3d26, 2, 1);
     TestComplete(0x3d45, 2, 1);
     TestComplete(0x3d4f, 2, 1);
     TestComplete(0x3d56, 2, 1);
     TestComplete(0x3d5f, 2, 1);
     TestComplete(0x3da6, 2, 1);
     TestComplete(0x3db2, 2, 1);
     TestComplete(0x3dbe, 2, 1);
     TestComplete(0x3dc6, 2, 1);
     TestComplete(0x3de0, 2, 1);
     TestComplete(0x3e03, 2, 1);
     TestComplete(0x3e21, 2, 1);
     TestComplete(0x3e31, 2, 1);
     TestComplete(0x3e3c, 2, 1);
     TestComplete(0x3e46, 2, 1);
     TestComplete(0x3e82, 2, 1);
     TestComplete(0x3e8f, 2, 1);
     TestComplete(0x3e92, 2, 1);
     TestComplete(0x3e94, 2, 1);
     TestComplete(0x3ea4, 2, 1);
     TestComplete(0x3ebb, 2, 1);
     TestComplete(0x3ec8, 2, 1);
     TestComplete(0x3ef6, 2, 1);
     TestComplete(0x3efa, 2, 1);
     TestComplete(0x3f1f, 2, 1);
     TestComplete(0x3f35, 2, 1);
     TestComplete(0x3f4f, 2, 1);
     TestComplete(0x3f8b, 2, 1);
     TestComplete(0x3fce, 2, 1);
     TestComplete(0x3fcf, 2, 1);
     TestComplete(0x3fdb, 2, 1);
     TestComplete(0x4001, 2, 1);
     TestComplete(0x4008, 2, 1);
     TestComplete(0x401d, 2, 1);
     TestComplete(0x4049, 2, 1);
     TestComplete(0x406d, 2, 1);
     TestComplete(0x4083, 2, 1);
     TestComplete(0x408e, 2, 1);
     TestComplete(0x40a2, 2, 1);
     TestComplete(0x40ae, 2, 1);
     TestComplete(0x4121, 2, 1);
     TestComplete(0x412f, 2, 1);
     TestComplete(0x413f, 2, 1);
     TestComplete(0x4148, 2, 1);
     TestComplete(0x4156, 2, 1);
     TestComplete(0x4177, 2, 1);
     TestComplete(0x419f, 2, 1);
     TestComplete(0x41b2, 2, 1);
     TestComplete(0x41cb, 2, 1);
     TestComplete(0x41d1, 2, 1);
     TestComplete(0x41e7, 2, 1);
     TestComplete(0x41fb, 2, 1);
     TestComplete(0x420b, 2, 1);
     TestComplete(0x423e, 2, 1);
     TestComplete(0x4261, 2, 1);
     TestComplete(0x426d, 2, 1);
     TestComplete(0x4276, 2, 1);
     TestComplete(0x4281, 2, 1);
     TestComplete(0x428c, 2, 1);
     TestComplete(0x42a8, 2, 1);
     TestComplete(0x42aa, 2, 1);
     TestComplete(0x42b7, 2, 1);
     TestComplete(0x42b9, 2, 1);
     TestComplete(0x4321, 2, 1);
     TestComplete(0x4338, 2, 1);
     TestComplete(0x4350, 2, 1);
     TestComplete(0x4371, 2, 1);
     TestComplete(0x4376, 2, 1);
     TestComplete(0x4403, 2, 1);
     TestComplete(0x4444, 2, 1);
     TestComplete(0x4474, 2, 1);
     TestComplete(0x447d, 2, 1);
     TestComplete(0x44a4, 2, 1);
     TestComplete(0x44b2, 2, 1);
     TestComplete(0x44c2, 2, 1);
     TestComplete(0x44cb, 2, 1);
     TestComplete(0x44d6, 2, 1);
     TestComplete(0x4525, 2, 1);
     TestComplete(0x453b, 2, 1);
     TestComplete(0x456a, 2, 1);
     TestComplete(0x459d, 2, 1);
     TestComplete(0x45ac, 2, 1);
     TestComplete(0x45b0, 2, 1);
     TestComplete(0x45b3, 2, 1);
     TestComplete(0x45df, 2, 1);
     TestComplete(0x4617, 2, 1);
     TestComplete(0x463a, 2, 1);
     TestComplete(0x4654, 2, 1);
     TestComplete(0x467e, 2, 1);
     TestComplete(0x46c8, 2, 1);
     TestComplete(0x46c9, 2, 1);
     TestComplete(0x473d, 2, 1);
     TestComplete(0x4779, 2, 1);
     TestComplete(0x4797, 2, 1);
     TestComplete(0x47e5, 2, 1);
     TestComplete(0x47f1, 2, 1);
     TestComplete(0x4819, 2, 1);
     TestComplete(0x4857, 2, 1);
     TestComplete(0x485b, 2, 1);
     TestComplete(0x4881, 2, 1);
     TestComplete(0x489b, 2, 1);
     TestComplete(0x48ac, 2, 1);
     TestComplete(0x48fa, 2, 1);
     TestComplete(0x4909, 2, 1);
     TestComplete(0x4930, 2, 1);
     TestComplete(0x4938, 2, 1);
     TestComplete(0x493b, 2, 1);
     TestComplete(0x4950, 2, 1);
     TestComplete(0x4995, 2, 1);
     TestComplete(0x4998, 2, 1);
     TestComplete(0x49b6, 2, 1);
     TestComplete(0x4a7e, 2, 1);
     TestComplete(0x4a99, 2, 1);
     TestComplete(0x4ac5, 2, 1);
     TestComplete(0x4acb, 2, 1);
     TestComplete(0x4b01, 2, 1);
     TestComplete(0x4b0b, 2, 1);
     TestComplete(0x4b40, 2, 1);
     TestComplete(0x4b71, 2, 1);
     TestComplete(0x4bfd, 2, 1);
     TestComplete(0x4bff, 2, 1);
     TestComplete(0x4c2e, 2, 1);
     TestComplete(0x4ca4, 2, 1);
     TestComplete(0x4ce9, 2, 1);
     TestComplete(0x4d15, 2, 1);
     TestComplete(0x4d44, 2, 1);
     TestComplete(0x4d5a, 2, 1);
     TestComplete(0x4dc7, 2, 1);
     TestComplete(0x4e56, 2, 1);
     TestComplete(0x4ea5, 2, 1);
     TestComplete(0x4f1c, 2, 1);
     TestComplete(0x4f21, 2, 1);
     TestComplete(0x4f7d, 2, 1);
     TestComplete(0x4f86, 2, 1);
     TestComplete(0x4f88, 2, 1);
     TestComplete(0x4fd4, 2, 1);
     TestComplete(0x502a, 2, 1);
     TestComplete(0x5035, 2, 1);
     TestComplete(0x5039, 2, 1);
     TestComplete(0x503b, 2, 1);
     TestComplete(0x5061, 2, 1);
     TestComplete(0x5068, 2, 1);
     TestComplete(0x5072, 2, 1);
     TestComplete(0x50a1, 2, 1);
     TestComplete(0x50d5, 2, 1);
     TestComplete(0x50fe, 2, 1);
     TestComplete(0x5100, 2, 1);
     TestComplete(0x5115, 2, 1);
     TestComplete(0x5143, 2, 1);
     TestComplete(0x5156, 2, 1);
     TestComplete(0x5162, 2, 1);
     TestComplete(0x516c, 2, 1);
     TestComplete(0x51d3, 2, 1);
     TestComplete(0x51f0, 2, 1);
     TestComplete(0x51f9, 2, 1);
     TestComplete(0x5252, 2, 1);
     TestComplete(0x5266, 2, 1);
     TestComplete(0x52ca, 2, 1);
     TestComplete(0x52d7, 2, 1);
     TestComplete(0x52f5, 2, 1);
     TestComplete(0x52f6, 2, 1);
     TestComplete(0x5307, 2, 1);
     TestComplete(0x531e, 2, 1);
     TestComplete(0x532b, 2, 1);
     TestComplete(0x5352, 2, 1);
     TestComplete(0x538d, 2, 1);
     TestComplete(0x5391, 2, 1);
     TestComplete(0x53fc, 2, 1);
     TestComplete(0x5407, 2, 1);
     TestComplete(0x5416, 2, 1);
     TestComplete(0x5423, 2, 1);
     TestComplete(0x5436, 2, 1);
     TestComplete(0x5452, 2, 1);
     TestComplete(0x5485, 2, 1);
     TestComplete(0x5494, 2, 1);
     TestComplete(0x5497, 2, 1);
     TestComplete(0x54e3, 2, 1);
     TestComplete(0x5522, 2, 1);
     TestComplete(0x552b, 2, 1);
     TestComplete(0x552e, 2, 1);
     TestComplete(0x5537, 2, 1);
     TestComplete(0x5555, 2, 1);
     TestComplete(0x557c, 2, 1);
     TestComplete(0x5596, 2, 1);
     TestComplete(0x55be, 2, 1);
     TestComplete(0x55cc, 2, 1);
     TestComplete(0x55f2, 2, 1);
     TestComplete(0x5609, 2, 1);
     TestComplete(0x5621, 2, 1);
     TestComplete(0x5634, 2, 1);
     TestComplete(0x5653, 2, 1);
     TestComplete(0x5662, 2, 1);
     TestComplete(0x5675, 2, 1);
     TestComplete(0x567d, 2, 1);
     TestComplete(0x5694, 2, 1);
     TestComplete(0x56c3, 2, 1);
     TestComplete(0x56c5, 2, 1);
     TestComplete(0x5707, 2, 1);
     TestComplete(0x570a, 2, 1);
     TestComplete(0x5715, 2, 1);
     TestComplete(0x5717, 2, 1);
     TestComplete(0x5724, 2, 1);
     TestComplete(0x572a, 2, 1);
     TestComplete(0x573d, 2, 1);
     TestComplete(0x57be, 2, 1);
     TestComplete(0x57cb, 2, 1);
     TestComplete(0x57f1, 2, 1);
     TestComplete(0x582c, 2, 1);
     TestComplete(0x5852, 2, 1);
     TestComplete(0x5860, 2, 1);
     TestComplete(0x58d2, 2, 1);
     TestComplete(0x58d9, 2, 1);
     TestComplete(0x58f6, 2, 1);
     TestComplete(0x5900, 2, 1);
     TestComplete(0x59e2, 2, 1);
     TestComplete(0x59f7, 2, 1);
     TestComplete(0x5a07, 2, 1);
     TestComplete(0x5a3c, 2, 1);
     TestComplete(0x5a9b, 2, 1);
     TestComplete(0x5ac2, 2, 1);
     TestComplete(0x5aff, 2, 1);
     TestComplete(0x5b47, 2, 1);
     TestComplete(0x5b53, 2, 1);
     TestComplete(0x5b6a, 2, 1);
     TestComplete(0x5bb6, 2, 1);
     TestComplete(0x5bc0, 2, 1);
     TestComplete(0x5bd0, 2, 1);
     TestComplete(0x5bf2, 2, 1);
     TestComplete(0x5c3b, 2, 1);
     TestComplete(0x5c45, 2, 1);
     TestComplete(0x5c54, 2, 1);
     TestComplete(0x5c58, 2, 1);
     TestComplete(0x5c65, 2, 1);
     TestComplete(0x5c80, 2, 1);
     TestComplete(0x5c8a, 2, 1);
     TestComplete(0x5c9a, 2, 1);
     TestComplete(0x5ca7, 2, 1);
     TestComplete(0x5caa, 2, 1);
     TestComplete(0x5cbe, 2, 1);
     TestComplete(0x5cf9, 2, 1);
     TestComplete(0x5cff, 2, 1);
     TestComplete(0x5d79, 2, 1);
     TestComplete(0x5d7d, 2, 1);
     TestComplete(0x5d96, 2, 1);
     TestComplete(0x5da6, 2, 1);
     TestComplete(0x5de2, 2, 1);
     TestComplete(0x5df5, 2, 1);
     TestComplete(0x5e01, 2, 1);
     TestComplete(0x5e3a, 2, 1);
     TestComplete(0x5e67, 2, 1);
     TestComplete(0x5e81, 2, 1);
     TestComplete(0x5ea2, 2, 1);
     TestComplete(0x5ea9, 2, 1);
     TestComplete(0x5eaa, 2, 1);
     TestComplete(0x5ee2, 2, 1);
     TestComplete(0x5f03, 2, 1);
     TestComplete(0x5f06, 2, 1);
     TestComplete(0x5f11, 2, 1);
     TestComplete(0x5f27, 2, 1);
     TestComplete(0x5f2c, 2, 1);
     TestComplete(0x5f32, 2, 1);
     TestComplete(0x5f44, 2, 1);
     TestComplete(0x5f5f, 2, 1);
     TestComplete(0x5f85, 2, 1);
     TestComplete(0x5fad, 2, 1);
     TestComplete(0x5fb1, 2, 1);
     TestComplete(0x5fe7, 2, 1);
     TestComplete(0x6036, 2, 1);
     TestComplete(0x6039, 2, 1);
     TestComplete(0x605d, 2, 1);
     TestComplete(0x608c, 2, 1);
     TestComplete(0x60B2, 2, 1);
     TestComplete(0x60b8, 2, 1);
     TestComplete(0x60c4, 2, 1);
     TestComplete(0x60eb, 2, 1);
     TestComplete(0x60f0, 2, 1);
     TestComplete(0x60f8, 2, 1);
     TestComplete(0x610e, 2, 1);
     TestComplete(0x613e, 2, 1);
     TestComplete(0x613f, 2, 1);
     TestComplete(0x6140, 2, 1);
     TestComplete(0x61c4, 2, 1);
     TestComplete(0x61dc, 2, 1);
     TestComplete(0x61fb, 2, 1);
     TestComplete(0x623d, 2, 1);
     TestComplete(0x6241, 2, 1);
     TestComplete(0x6267, 2, 1);
     TestComplete(0x62c8, 2, 1);
     TestComplete(0x62e2, 2, 1);
     TestComplete(0x62e6, 2, 1);
     TestComplete(0x62e8, 2, 1);
     TestComplete(0x62f8, 2, 1);
     TestComplete(0x62fc, 2, 1);
     TestComplete(0x6300, 2, 1);
     TestComplete(0x6320, 2, 1);
     TestComplete(0x637c, 2, 1);
     TestComplete(0x63a8, 2, 1);
     TestComplete(0x63bb, 2, 1);
     TestComplete(0x642b, 2, 1);
     TestComplete(0x647f, 2, 1);
     TestComplete(0x64ac, 2, 1);
     TestComplete(0x64b6, 2, 1);
     TestComplete(0x64fb, 2, 1);
     TestComplete(0x6517, 2, 1);
     TestComplete(0x6532, 2, 1);
     TestComplete(0x657f, 2, 1);
     TestComplete(0x6594, 2, 1);
     TestComplete(0x6595, 2, 1);
     TestComplete(0x659a, 2, 1);
     TestComplete(0x65c2, 2, 1);
     TestComplete(0x65c9, 2, 1);
     TestComplete(0x65cf, 2, 1);
     TestComplete(0x65de, 2, 1);
     TestComplete(0x65f9, 2, 1);
     TestComplete(0x662c, 2, 1);
     TestComplete(0x662d, 2, 1);
     TestComplete(0x665c, 2, 1);
     TestComplete(0x6666, 2, 1);
     TestComplete(0x6675, 2, 1);
     TestComplete(0x669b, 2, 1);
     TestComplete(0x66ad, 2, 1);
     TestComplete(0x66d3, 2, 1);
     TestComplete(0x66f2, 2, 1);
     TestComplete(0x6710, 2, 1);
     TestComplete(0x6713, 2, 1);
     TestComplete(0x6723, 2, 1);
     TestComplete(0x676e, 2, 1);
     TestComplete(0x67a2, 2, 1);
     TestComplete(0x67a4, 2, 1);
     TestComplete(0x67bc, 2, 1);
     TestComplete(0x6829, 2, 1);
     TestComplete(0x6866, 2, 1);
     TestComplete(0x6867, 2, 1);
     TestComplete(0x689f, 2, 1);
     TestComplete(0x68ae, 2, 1);
     TestComplete(0x68be, 2, 1);
     TestComplete(0x68d1, 2, 1);
     TestComplete(0x68d3, 2, 1);
     TestComplete(0x6930, 2, 1);
     TestComplete(0x6948, 2, 1);
     TestComplete(0x6963, 2, 1);
     TestComplete(0x699d, 2, 1);
     TestComplete(0x69bf, 2, 1);
     TestComplete(0x69f5, 2, 1);
     TestComplete(0x69fd, 2, 1);
     TestComplete(0x6a05, 2, 1);
     TestComplete(0x6a33, 2, 1);
     TestComplete(0x6a60, 2, 1);
     TestComplete(0x6a74, 2, 1);
     TestComplete(0x6aa0, 2, 1);
     TestComplete(0x6ac5, 2, 1);
     TestComplete(0x6acf, 2, 1);
     TestComplete(0x6ae4, 2, 1);
     TestComplete(0x6aef, 2, 1);
     TestComplete(0x6b2b, 2, 1);
     TestComplete(0x6b35, 2, 1);
     TestComplete(0x6b39, 2, 1);
     TestComplete(0x6b4b, 2, 1);
     TestComplete(0x6b56, 2, 1);
     TestComplete(0x6b68, 2, 1);
     TestComplete(0x6b89, 2, 1);
     TestComplete(0x6b94, 2, 1);
     TestComplete(0x6bb1, 2, 1);
     TestComplete(0x6c12, 2, 1);
     TestComplete(0x6c4e, 2, 1);
     TestComplete(0x6c5d, 2, 1);
     TestComplete(0x6c76, 2, 1);
     TestComplete(0x6c95, 2, 1);
     TestComplete(0x6c9e, 2, 1);
     TestComplete(0x6cd0, 2, 1);
     TestComplete(0x6ce5, 2, 1);
     TestComplete(0x6cec, 2, 1);
     TestComplete(0x6d38, 2, 1);
     TestComplete(0x6db3, 2, 1);
     TestComplete(0x6dc4, 2, 1);
     TestComplete(0x6dcc, 2, 1);
     TestComplete(0x6dd3, 2, 1);
     TestComplete(0x6e3e, 2, 1);
     TestComplete(0x6e3f, 2, 1);
     TestComplete(0x6e52, 2, 1);
     TestComplete(0x6e5b, 2, 1);
     TestComplete(0x6e5e, 2, 1);
     TestComplete(0x6e97, 2, 1);
     TestComplete(0x6eba, 2, 1);
     TestComplete(0x6f36, 2, 1);
     TestComplete(0x6f37, 2, 1);
     TestComplete(0x6f65, 2, 1);
     TestComplete(0x6f7a, 2, 1);
     TestComplete(0x6f7c, 2, 1);
     TestComplete(0x6f84, 2, 1);
     TestComplete(0x6f9c, 2, 1);
     TestComplete(0x6fa2, 2, 1);
     TestComplete(0x6fc1, 2, 1);
     TestComplete(0x6fd6, 2, 1);
     TestComplete(0x6feb, 2, 1);
     TestComplete(0x6ff3, 2, 1);
     TestComplete(0x7007, 2, 1);
     TestComplete(0x7025, 2, 1);
     TestComplete(0x7067, 2, 1);
     TestComplete(0x70A3, 2, 1);
     TestComplete(0x70ba, 2, 1);
     TestComplete(0x70c7, 2, 1);
     TestComplete(0x70ce, 2, 1);
     TestComplete(0x70d1, 2, 1);
     TestComplete(0x70d2, 2, 1);
     TestComplete(0x70d8, 2, 1);
     TestComplete(0x7108, 2, 1);
     TestComplete(0x7109, 2, 1);
     TestComplete(0x7113, 2, 1);
     TestComplete(0x711d, 2, 1);
     TestComplete(0x7125, 2, 1);
     TestComplete(0x714d, 2, 1);
     TestComplete(0x7172, 2, 1);
     TestComplete(0x7184, 2, 1);
     TestComplete(0x71a2, 2, 1);
     TestComplete(0x71a5, 2, 1);
     TestComplete(0x71c1, 2, 1);
     TestComplete(0x71cd, 2, 1);
     TestComplete(0x71e8, 2, 1);
     TestComplete(0x71ea, 2, 1);
     TestComplete(0x71eb, 2, 1);
     TestComplete(0x71f9, 2, 1);
     TestComplete(0x7206, 2, 1);
     TestComplete(0x721f, 2, 1);
     TestComplete(0x7252, 2, 1);
     TestComplete(0x7258, 2, 1);
     TestComplete(0x7270, 2, 1);
     TestComplete(0x7277, 2, 1);
     TestComplete(0x727a, 2, 1);
     TestComplete(0x72a0, 2, 1);
     TestComplete(0x72a5, 2, 1);
     TestComplete(0x72b1, 2, 1);
     TestComplete(0x72dc, 2, 1);
     TestComplete(0x7344, 2, 1);
     TestComplete(0x734d, 2, 1);
     TestComplete(0x735d, 2, 1);
     TestComplete(0x7382, 2, 1);
     TestComplete(0x739c, 2, 1);
     TestComplete(0x73d6, 2, 1);
     TestComplete(0x7415, 2, 1);
     TestComplete(0x747e, 2, 1);
     TestComplete(0x7483, 2, 1);
     TestComplete(0x7497, 2, 1);
     TestComplete(0x74c9, 2, 1);
     TestComplete(0x755c, 2, 1);
     TestComplete(0x755f, 2, 1);
     TestComplete(0x7586, 2, 1);
     TestComplete(0x75bf, 2, 1);
     TestComplete(0x75ee, 2, 1);
     TestComplete(0x7606, 2, 1);
     TestComplete(0x7619, 2, 1);
     TestComplete(0x7628, 2, 1);
     TestComplete(0x763a, 2, 1);
     TestComplete(0x765a, 2, 1);
     TestComplete(0x7677, 2, 1);
     TestComplete(0x768e, 2, 1);
     TestComplete(0x7697, 2, 1);
     TestComplete(0x76ae, 2, 1);
     TestComplete(0x76b4, 2, 1);
     TestComplete(0x76da, 2, 1);
     TestComplete(0x76e1, 2, 1);
     TestComplete(0x770d, 2, 1);
     TestComplete(0x7740, 2, 1);
     TestComplete(0x776d, 2, 1);
     TestComplete(0x7777, 2, 1);
     TestComplete(0x7782, 2, 1);
     TestComplete(0x7794, 2, 1);
     TestComplete(0x77d7, 2, 1);
     TestComplete(0x77e5, 2, 1);
     TestComplete(0x77f1, 2, 1);
     TestComplete(0x7806, 2, 1);
     TestComplete(0x7820, 2, 1);
     TestComplete(0x7857, 2, 1);
     TestComplete(0x788b, 2, 1);
     TestComplete(0x7892, 2, 1);
     TestComplete(0x78ad, 2, 1);
     TestComplete(0x78ce, 2, 1);
     TestComplete(0x7915, 2, 1);
     TestComplete(0x7942, 2, 1);
     TestComplete(0x7946, 2, 1);
     TestComplete(0x794b, 2, 1);
     TestComplete(0x7953, 2, 1);
     TestComplete(0x797e, 2, 1);
     TestComplete(0x7987, 2, 1);
     TestComplete(0x79f5, 2, 1);
     TestComplete(0x7a08, 2, 1);
     TestComplete(0x7a11, 2, 1);
     TestComplete(0x7a3b, 2, 1);
     TestComplete(0x7a44, 2, 1);
     TestComplete(0x7a5d, 2, 1);
     TestComplete(0x7a89, 2, 1);
     TestComplete(0x7a9c, 2, 1);
     TestComplete(0x7a9d, 2, 1);
     TestComplete(0x7aaf, 2, 1);
     TestComplete(0x7b5d, 2, 1);
     TestComplete(0x7b69, 2, 1);
     TestComplete(0x7b6d, 2, 1);
     TestComplete(0x7b75, 2, 1);
     TestComplete(0x7b77, 2, 1);
     TestComplete(0x7b8f, 2, 1);
     TestComplete(0x7bb3, 2, 1);
     TestComplete(0x7bba, 2, 1);
     TestComplete(0x7bcf, 2, 1);
     TestComplete(0x7c1a, 2, 1);
     TestComplete(0x7c23, 2, 1);
     TestComplete(0x7c4f, 2, 1);
     TestComplete(0x7c67, 2, 1);
     TestComplete(0x7c6b, 2, 1);
     TestComplete(0x7c6c, 2, 1);
     TestComplete(0x7c71, 2, 1);
     TestComplete(0x7c93, 2, 1);
     TestComplete(0x7d01, 2, 1);
     TestComplete(0x7d07, 2, 1);
     TestComplete(0x7d14, 2, 1);
     TestComplete(0x7d15, 2, 1);
     TestComplete(0x7d24, 2, 1);
     TestComplete(0x7d37, 2, 1);
     TestComplete(0x7d41, 2, 1);
     TestComplete(0x7d59, 2, 1);
     TestComplete(0x7d6e, 2, 1);
     TestComplete(0x7d86, 2, 1);
     TestComplete(0x7d9c, 2, 1);
     TestComplete(0x7dde, 2, 1);
     TestComplete(0x7de0, 2, 1);
     TestComplete(0x7dee, 2, 1);
     TestComplete(0x7e00, 2, 1);
     TestComplete(0x7e16, 2, 1);
     TestComplete(0x7e1d, 2, 1);
     TestComplete(0x7e2a, 2, 1);
     TestComplete(0x7e60, 2, 1);
     TestComplete(0x7e65, 2, 1);
     TestComplete(0x7e87, 2, 1);
     TestComplete(0x7ed0, 2, 1);
     TestComplete(0x7ed2, 2, 1);
     TestComplete(0x7f0f, 2, 1);
     TestComplete(0x7f45, 2, 1);
     TestComplete(0x7f52, 2, 1);
     TestComplete(0x7f57, 2, 1);
     TestComplete(0x7f61, 2, 1);
     TestComplete(0x7f64, 2, 1);
     TestComplete(0x7f94, 2, 1);
     TestComplete(0x7f97, 2, 1);
     TestComplete(0x7fe7, 2, 1);
     TestComplete(0x7ff2, 2, 1);
     TestComplete(0x8888, 2, 1);
     TestComplete(0x892D, 2, 1);
     TestComplete(0x893C, 2, 1);
     TestComplete(0x9999, 2, 1);
     TestComplete(0xaaaa, 2, 1);
     TestComplete(0xaea6, 2, 1);
     TestComplete(0xBB41, 2, 1);
     TestComplete(0xbbbb, 2, 1);
     TestComplete(0xbeef, 2, 1);
     TestComplete(0xc698, 2, 1);
     TestComplete(0xcafe, 2, 1);
     TestComplete(0xcccc, 2, 1);
     TestComplete(0xD3B5, 2, 1);
     TestComplete(0xdddd, 2, 1);
     TestComplete(0xdead, 2, 1);
     TestComplete(0xdeed, 2, 1);
     TestComplete(0xe43f, 2, 1);
     TestComplete(0xebe9, 2, 1);
     TestComplete(0xeeee, 2, 1);
     TestComplete(0xF61A, 2, 1);
     TestComplete(0xFB7F, 2, 1);
     TestComplete(0xFB81, 2, 1);
     TestComplete(0xFB82, 2, 1);
     TestComplete(0xffff, 2, 1);
     xTestComplete(0x036d, 2, 1);
     xTestComplete(0x0378, 2, 1);
     xTestComplete(0x03a5, 2, 1);
     xTestComplete(0x03d0, 2, 1);
     xTestComplete(0x0425, 2, 1);
     xTestComplete(0x213d, 2, 1);
     xTestComplete(0x2144, 2, 1);
     xTestComplete(0x21d7, 2, 1);
     xTestComplete(0x21e9, 2, 1);
     xTestComplete(0x21fd, 2, 1);
     xTestComplete(0x2219, 2, 1);
     xTestComplete(0x22cf, 2, 1);
     xTestComplete(0x22db, 2, 1);
     xTestComplete(0x22e1, 2, 1);
     xTestComplete(0x22ec, 2, 1);
     xTestComplete(0x22f1, 2, 1);
     xTestComplete(0x2357, 2, 1);
     xTestComplete(0x23a2, 2, 1);
     xTestComplete(0x23c2, 2, 1);
     xTestComplete(0x23C6, 2, 1);
     xTestComplete(0x2451, 2, 1);
     xTestComplete(0x2475, 2, 1);
     */

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

    if(success) ; // printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
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

int TestSuite::testWorld(uint16 seed, WorldSize size, uint32 *sample) {
    int success = 1;

    YodaDocument *doc = new YodaDocument(DATA_PATH);
    doc->logging = 0;

    WorldGenerator generator(doc);
    generator.generateWorld(seed, (WorldSize)size, HOTH);

    for(int i=0; i < 100; i++) {
        if(!(generator.worldThings[i].zone_type == sample[i*4+0])
           || !(generator.worldThings[i].zone_id == sample[i*4+1])
           || !(generator.worldThings[i].findItemID == sample[i*4+2])
           || !(generator.worldThings[i].requiredItemID == sample[i*4+3])) {
            success = 0;
            break;
        }
    }

    if(success) ; // printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    else printf("[FAIL] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));

    delete doc;

    return success;
}

int TestSuite::testCompleteWorld(uint16 seed, WorldSize size, Planet planet, uint16* sample) {
    int success = 1;

    YodaDocument *doc = new YodaDocument(DATA_PATH);
    doc->logging = 1;

    WorldGenerator generator(doc);
    generator.generateWorld(seed, (WorldSize)size, planet);


    for(int i=0; i < 100; i++) {
        if(
           !(generator.worldThings[i].zone_id == sample[i*10]) ||
           !(generator.worldThings[i].zone_type == sample[i*10+1]) ||
           !(generator.worldThings[i].findItemID == sample[i*10+6]) ||
           !(generator.worldThings[i].requiredItemID == sample[i*10+4]) ||
           !(generator.worldThings[i].unknown606 == sample[i*10+5]) ||
           !(generator.worldThings[i].unknown612 == sample[i*10+7])
           ) {
            success = 0;
            break;
        }
    }
    
    if(!success) {
        FILE *real = fopen("/Users/chris/Desktop/r.txt", "w+");
        FILE *sam = fopen("/Users/chris/Desktop/s.txt", "w+");
        
        for(int y=0; y < 10; y++){
            for(int x=0; x < 10; x++){
                int idx = x + y * 10;
                fprintf(sam, "%4d ", (int16)sample[idx*10+5]);
                fprintf(real, "%4d ", (int16)generator.worldThings[idx].unknown606);
            }
            
            fprintf(real, "\n");
            fprintf(sam, "\n");
        }
        
        fclose(real);
        fclose(sam);
        
        printf("\nFAIL\n");
    }
    
    if(success) printf("[OK] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    else printf("[FAIL] 0x%04x %s\n", seed, size == 1 ? "small" : (size == 2 ? "medium" : "large"));
    
    delete doc;
    
    return success;
}
