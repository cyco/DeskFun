typedef enum {
    ZONETYPE_Empty          = 1,
    ZONETYPE_BlockadeNorth  = 2,
    ZONETYPE_BlockadeSouth  = 3,
    ZONETYPE_BlockadeEast   = 4,
    ZONETYPE_BlockadeWest   = 5,
    ZONETYPE_TravelStart = 6,
    ZONETYPE_TravelEnd   = 7,
    ZONETYPE_Room  = 8,
    ZONETYPE_Load  = 9,
    ZONETYPE_Goal  = 10,
    ZONETYPE_Town  = 11,
    ZONETYPE_Win   = 13,
    ZONETYPE_Lose  = 14,
    ZONETYPE_Trade        = 15,
    ZONETYPE_Use          = 16,
    ZONETYPE_Find         = 17,
    ZONETYPE_FindTheForce = 18,
} ZONE_TYPE;