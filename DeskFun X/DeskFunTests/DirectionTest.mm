//
//  DirectionTest.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "Direction.hpp"

using namespace Direction;

@interface DirectionTests : XCTestCase
@end

@implementation DirectionTests

- (void)testNormalize
{
    XCTAssert(Normalize(0)   == 0, @"Normalize");
    XCTAssert(Normalize(90)  == 90, @"Normalize");
    XCTAssert(Normalize(180) == 180, @"Normalize");
    XCTAssert(Normalize(360) == 0, @"Normalize full circle");
    XCTAssert(Normalize(450) == 90, @"Normalize");
    XCTAssert(Normalize(4320) == 0, @"Normalize");
    XCTAssert(Normalize(-90) == 270, @"Normalize negative value");
    XCTAssert(Normalize(-0)  == 0, @"Normalize negative zero");
}

- (void)testZeroDistance
{
    float dir = North;
    Pointf p = RelativeCoordinates(dir, 0);

    XCTAssert(p.x == 0, @"X-Coordinate");
    XCTAssert(p.y == 0, @"Y-Coordinate");

    p = RelativeCoordinates(123.2, 0);
    XCTAssert(p.x == 0, @"X-Coordinate");
    XCTAssert(p.y == 0, @"Y-Coordinate");
}

- (void)testNorth
{
    Pointf p = RelativeCoordinates(North, 1);

    XCTAssertEqualWithAccuracy(p.x, 0, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, -1, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(North, 2);

    XCTAssertEqualWithAccuracy(p.x, 0,  FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, -2, FLT_EPSILON, @"Y-Coordinate");
}


- (void)testEast
{
    Pointf p = RelativeCoordinates(East, 1);

    XCTAssertEqualWithAccuracy(p.x, 1, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 0, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(East, 2);

    XCTAssertEqualWithAccuracy(p.x, 2, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 0, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testSouth
{
    Pointf p = RelativeCoordinates(South, 1);

    XCTAssertEqualWithAccuracy(p.x, 0, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 1, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(South, 2);

    XCTAssertEqualWithAccuracy(p.x, 0, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 2, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testSouthEast
{
    Pointf p = RelativeCoordinates(SouthEast, 1);
    XCTAssertEqualWithAccuracy(p.x, 1, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 1, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(SouthEast, 2);
    XCTAssertEqualWithAccuracy(p.x, 2, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 2, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testWest
{
    Pointf p;

    p = RelativeCoordinates(West, 1);
    XCTAssertEqualWithAccuracy(p.x, -1, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 0, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(West, 2);
    XCTAssertEqualWithAccuracy(p.x, -2, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 0, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testNorthWest
{
    Pointf p = RelativeCoordinates(NorthWest, 1);
    XCTAssertEqualWithAccuracy(p.x, -1, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, -1, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(NorthWest, 2);
    XCTAssertEqualWithAccuracy(p.x, -2, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, -2, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testSouthWest
{
    Pointf p = RelativeCoordinates(SouthWest, 1);
    XCTAssertEqualWithAccuracy(p.x, -1, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 1, FLT_EPSILON, @"Y-Coordinate");

    p = RelativeCoordinates(SouthWest, 2);
    XCTAssertEqualWithAccuracy(p.x, -2, FLT_EPSILON, @"X-Coordinate");
    XCTAssertEqualWithAccuracy(p.y, 2, FLT_EPSILON, @"Y-Coordinate");
}

- (void)testHorizontalCombinations
{
    Pointf p1, p2;

    p1 = RelativeCoordinates(West, 0);
    p2 = RelativeCoordinates(East, 0);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(West, 1);
    p2 = RelativeCoordinates(East, -1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(West, 1);
    p2 = RelativeCoordinates(East, -1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(East, -1);
    p2 = RelativeCoordinates(West, 1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");
}

- (void)testVerticalCombinations
{
    Pointf p1, p2;

    p1 = RelativeCoordinates(North, 0);
    p2 = RelativeCoordinates(South, 0);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(North, 1);
    p2 = RelativeCoordinates(South, -1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(North, 1);
    p2 = RelativeCoordinates(South, -1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");

    p1 = RelativeCoordinates(South, -1);
    p2 = RelativeCoordinates(North, 1);
    XCTAssertEqualWithAccuracy(p1.x, p2.x, FLT_EPSILON, @"X-Coordinates");
    XCTAssertEqualWithAccuracy(p1.y, p2.y, FLT_EPSILON, @"X-Coordinates");
}

- (void)testNormalization
{
    XCTAssertEqualWithAccuracy(Confine(1), 0, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(-1), 0, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(34), 45, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(359), 0, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(179.2), 180, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(22.5), 45, FLT_EPSILON);
    XCTAssertEqualWithAccuracy(Confine(22.4), 0, FLT_EPSILON);
}

- (void)testUnidirectionalQuery
{
    XCTAssertTrue(isUnidirectional(0));
    XCTAssertTrue(isUnidirectional(90));
    XCTAssertTrue(isUnidirectional(180));
    XCTAssertTrue(isUnidirectional(360));

    XCTAssertFalse(isUnidirectional(23));
    XCTAssertFalse(isUnidirectional(45));
    XCTAssertFalse(isUnidirectional(-45));
    XCTAssertFalse(isUnidirectional(-23));
}
@end
