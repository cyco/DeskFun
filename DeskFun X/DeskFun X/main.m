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
    TestSuite suite;
    return suite.runTests();
    
    return NSApplicationMain(argc, argv);
}
