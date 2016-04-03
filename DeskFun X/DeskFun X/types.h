#ifndef REIMP_TYPES
#define REIMP_TYPES REIMP_TYPES

#define DeterminePuzzleLocationsCall1 0
#define DeterminePuzzleLocationsCall2 0
#define DeterminePuzzleLocationsCall3 0
#define DeterminePuzzleLocationsCall4 0
#define DeterminePuzzleLocationsCall5 0


#include "map.h"
typedef enum {
	WORLD_SIZE_SMALL = 1,
	WORLD_SIZE_MEDIUM = 2,
	WORLD_SIZE_LARGE = 3
} WORLD_SIZE;

typedef short int16;
typedef int int32;
typedef unsigned short uint16_t;
typedef unsigned short uint16;
#include "win_rand.h"
#endif