//
//  Direction.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef Desktop_Adventure_Direction_hpp
#define Desktop_Adventure_Direction_hpp

#include <math.h>
#include "EngineTypes.hpp"

namespace Direction {
    typedef enum : int {
        East = 0,
        SouthEast = 45,
        South = 90,
        SouthWest = 135,
        West = 180,
        NorthWest = 225,
        North = 270,
        NorthEast = 315,
    } _WellDefineDirections;

    static const float TooFar  = INFINITY;
    static const float TooNear = NAN;

    inline float Normalize(float direction) {
        int result = fmodf(direction, 360.0);
        if(result < 0) result += 360.0;
        return result;
    }

    inline int Confine(float direction) {
        return Normalize(roundf(direction/45.0) * 45.0);
    }

    inline bool isUnidirectional(float direction) {
        return Confine(direction) % 90 == 0;
    }

    inline float CalculateFromRelativePoint(Pointf relative) {
        if (relative.x < 0)
            return 180.0 - atanf(relative.y / -relative.x) * 180.0 / M_PI;
        else
            return atanf(relative.y / relative.x) * 180.0 / M_PI;
    }

    inline Pointf RelativeCoordinates(float direction, float distance) {
        float rad = direction * M_PI/180.0;
        return (Pointf){.x = distance * roundf(cosf(rad)),
                        .y = distance * roundf(sinf(rad))};
    }

    inline Pointf AddToPoint(Pointf p, float direction, float distance) {
        Pointf r = RelativeCoordinates(direction, distance);
        return (Pointf){.x=p.x + r.x, .y = p.y+r.y };
    }
}

#endif
