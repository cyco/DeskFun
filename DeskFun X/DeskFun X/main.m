//
//  main.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "TestSuite.hpp"
#include "win_rand.h"

int main(int argc, const char * argv[]) {
    /*
     win_srand(0x0711);
     for(int i=0; i < 100; i++) {
     printf("testWorld(0x%04x, %d, %d);\n", win_rand(), win_rand()%3+1, win_rand()%3+1);
     }
     */
    TestSuite suite;
    // return suite.runTests();

    return NSApplicationMain(argc, argv);
}
