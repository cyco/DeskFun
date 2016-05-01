//
//  MapGenerator.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 20/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "MapGenerator.hpp"

// TODO: get rid of macros
#define get(_x_,_y_) getFrom((_x_), (_y_), (int16*)map)
#define set(_x_,_y_,_v_) setTo(_x_,_y_,_v_,(int16*)map)
#define getPuzzle(_x_,_y_) getFrom((_x_), (_y_), (int16*)puzzles)
#define setPuzzle(_x_, _y_, _v_) setTo(_x_,_y_,_v_,(int16*)puzzles)

int MapGenerator::GetDistanceToCenter(int x, int y) {
    Message("Map::GetDistanceToCenter %dx%d\n", x, y);
    static int distances[] = {
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 1, 1, 2, 3, 4, 5,
        5, 4, 3, 2, 2, 2, 2, 3, 4, 5,
        5, 4, 3, 3, 3, 3, 3, 3, 4, 5,
        5, 4, 4, 4, 4, 4, 4, 4, 4, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5
    };
    return distances[x + 10 * y];
}
#pragma mark - lifecyle
MapGenerator::MapGenerator(WorldSize s) {
    size = s;
    map = new uint16[100];
    puzzles = new int16[100];
    puzzleCount = -1;
}

void MapGenerator::_clear() {
    for(int i=0; i < 100; i++) {
        map[i] = 0;
        puzzles[i] = -1;
    }
}

MapGenerator::~MapGenerator() {
    delete [] map;
    delete [] puzzles;
}

#pragma mark - Generation
bool MapGenerator::generate() {
    _clear();
    
    int transport_count = win_rand() % 3;
    int placed_transport_count = 0;
    int blockade_count = win_rand() % 4;
    int placed_blockade_count = 0;

    // Message("travels: %d, blockades: %d\n", transport_count, blockade_count);

    win_rand(); // waste a random number
    
    int spaceport_x_rand = win_rand();
    int spaceport_x = (spaceport_x_rand & 1) + 4;
    int spaceport_y_rand = win_rand();
    int spaceport_y = (spaceport_y_rand & 1) + 4;
    
    map[44] = MapType_EMPTY;
    map[45] = MapType_EMPTY;
    map[54] = MapType_EMPTY;
    map[55] = MapType_EMPTY;
    
    map[spaceport_x + 10 * spaceport_y] = MapType_SPACEPORT;
    
    int placed_puzzle_count = 4;
    int max1, base1, max2, base2, max3, base3, max4, base4;
    switch (size) {
        case WORLD_SIZE_SMALL:
            max1 = 8, base1 = 5;
            max2 = 6, base2 = 4;
            max3 = 1, base3 = 1;
            max4 = 1, base4 = 1;
            break;
        case WORLD_SIZE_MEDIUM:
            max1 = 9, base1 = 5;
            max2 = 9, base2 = 5;
            max3 = 8, base3 = 4;
            max4 = 8, base4 = 3;
            break;
        case WORLD_SIZE_LARGE:
            max1 = 12, base1 = 6;
            max2 = 12, base2 = 6;
            max3 = 11, base3 = 6;
            max4 = 11, base4 = 4;
            break;
    }
    
    
    int rand = win_rand();
    int items_to_place = transport_count + blockade_count + base1 + rand % (max1 - base1 + 1);
    if (items_to_place > 12) items_to_place = 12;

    determinePuzzleLocations(2,
                             items_to_place,
                             transport_count,
                             &placed_transport_count,
                             blockade_count,
                             &placed_blockade_count,
                             &placed_puzzle_count);

    determinePuzzleLocations(3,
                             base2 + win_rand() % (max2 - base2 + 1),
                             transport_count,
                             &placed_transport_count,
                             blockade_count,
                             &placed_blockade_count,
                             &placed_puzzle_count);

    determinePuzzleLocations(4,
                             base3 + win_rand() % (max3 - base3 + 1),
                             transport_count,
                             &placed_transport_count,
                             blockade_count,
                             &placed_blockade_count,
                             &placed_puzzle_count);

    additionalPuzzleLocations(win_rand() % (max4 - base4 + 1) + base4, &placed_puzzle_count);
    placeTransports(placed_transport_count);
    placeSomethingOnTheMap();

    puzzleCount = _countPuzzles();
    
    return true;
}

int MapGenerator::_countPuzzles() {
    int count = 0;
    for(int i=0; i < 100; i++) {
        if(puzzles[i] != -1) count++;
    }
    return count;
}

int MapGenerator::determinePuzzleLocations(signed int iteration, int puzzle_count_to_place, int transport_count, int *placed_transport_count_ref, int blockade_count, int *placed_blockade_count_ref, int *placed_puzzle_count_ref)
{
    Message("YodaDocument::PlacePuzzleLocations(%d, %d, %d, %d)\n", iteration, puzzle_count_to_place, transport_count, blockade_count);
    int base_x, alternate_x;
    int base_y, alternate_y;
    int variance, probablility;
    int threshold, threshold_2;
    switch ( iteration )
    {
        case 2:
            base_x = 3, alternate_x = 6;
            base_y = 3, alternate_y = 6;
            variance = 4;
            probablility = 9;
            threshold = 2;
            threshold_2 = 1;
            break;
        case 3:
            base_x = 2, alternate_x = 7;
            base_y = 2, alternate_y = 7;
            variance = 6;
            probablility = 4;
            threshold_2 = 3;
            threshold = 2;
            break;
        case 4:
            base_x = 1, alternate_x = 8;
            base_y = 1, alternate_y = 8;
            variance = 8;
            threshold = 1;
            probablility = 5;
            threshold_2 = 6;
            break;
        default: return 0;
    }
    
    for(int i=0; i <= 144 && puzzle_count_to_place > 0; i++)
    {
        int16 last_item = MapType_NONE;
        
        int x, y;
        if(win_rand() % 2) {
            x = win_rand() % 2 ? base_x : alternate_x;
            y = win_rand() % variance + base_y;
        } else {
            y = win_rand() % 2 ? base_y : alternate_y;
            x = win_rand() % variance + base_x;
        }
        
        int item_idx = x + 10 * y;
        if (map[item_idx]) continue;
        
        int16 item_before = get(x-1, y);
        int16 item_after = get(x+1, y);
        int16 item_above = get(x, y-1);
        int16 item_below = get(x, y+1);
        
        if (item_before != MapType_NONE && item_before != MapType_KEPT_FREE) {
            // Look at left neighbor
            last_item = map[item_idx-1];
            if ( item_before > MapType_TRAVEL_START && item_before != MapType_SPACEPORT && item_before != MapType_PUZZLE)
            {
                if((item_before == MapType_PUZZLE_CANDIDATE
                    || item_before == MapType_BLOCK_EAST)
                   
                   && ((item_above==MapType_NONE || item_above == MapType_KEPT_FREE)
                       && (item_below==MapType_NONE || item_below == MapType_KEPT_FREE)))
                {
                    map[item_idx] = MapType_PUZZLE_CANDIDATE;
                    map[item_idx-10] = MapType_KEPT_FREE;
                    map[item_idx+10] = MapType_KEPT_FREE;
                    
                    ++*placed_puzzle_count_ref;
                    --puzzle_count_to_place;
                }
                
                tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
                continue;
            }
            
            if (   item_before != MapType_TRAVEL_START
                && item_before != MapType_EMPTY
                && item_before != MapType_SPACEPORT
                && item_before != MapType_PUZZLE)
            {
                tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
                continue;
            }
            
            if ( blockade_count > *placed_blockade_count_ref && win_rand() % probablility < threshold )
            {
                if ( MapGenerator::GetDistanceToCenter(x - 1, y) < iteration )  {
                    if ( ( !item_above || item_above == MapType_KEPT_FREE )
                        && ( !item_below || item_below == MapType_KEPT_FREE ))
                    {
                        map[item_idx] = MapType_BLOCK_EAST;
                        --puzzle_count_to_place;
                        *placed_puzzle_count_ref += 2;
                        map[item_idx-10] = MapType_KEPT_FREE;
                        map[item_idx+10] = MapType_KEPT_FREE;
                        map[item_idx+1] = MapType_PUZZLE_CANDIDATE;
                        map[item_idx-9] = MapType_KEPT_FREE;
                        map[item_idx+11] = MapType_KEPT_FREE;
                        ++*placed_blockade_count_ref;
                    } else if ( item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                        map[item_idx] = MapType_EMPTY;
                        
                        ++*placed_puzzle_count_ref;
                        --puzzle_count_to_place;
                    }
                }
            } else if ( item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                map[item_idx] = MapType_EMPTY;
                ++*placed_puzzle_count_ref;
                --puzzle_count_to_place;
            }
        } else if (item_after != MapType_NONE && item_after != MapType_KEPT_FREE) { // Look at right neighbor
            last_item = map[item_idx+1];
            switch ( item_after ) {
                case MapType_PUZZLE_CANDIDATE:
                case MapType_BLOCK_WEST:
                    if ((item_above == 0 || item_above == MapType_KEPT_FREE )
                        && (item_below == 0 || item_below == MapType_KEPT_FREE )) {
                        
                        map[item_idx] = MapType_PUZZLE_CANDIDATE;
                        map[item_idx-10] = MapType_KEPT_FREE;
                        map[item_idx+10] = MapType_KEPT_FREE;
                        
                        ++*placed_puzzle_count_ref;
                        --puzzle_count_to_place;
                    }
                    /* intentional fallthrough */
                case MapType_INVALID:
                case MapType_NONE:
                case MapType_TRAVEL_END:
                case MapType_KEPT_FREE:
                case MapType_ISLAND:
                case MapType_BLOCK_SOUTH:
                case MapType_BLOCK_NORTH:
                case MapType_BLOCK_EAST:
                    tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
                    continue;
                case MapType_TRAVEL_START:
                case MapType_EMPTY:
                case MapType_SPACEPORT:
                case MapType_PUZZLE:
                    break;
            }
            
            if (blockade_count > *placed_blockade_count_ref && win_rand() % probablility < threshold )
            {
                int distance_is_less_than_iteration = MapGenerator::GetDistanceToCenter(x + 1, y) < iteration;
                if (distance_is_less_than_iteration && (!item_above || item_above == MapType_KEPT_FREE) &&
                    (!item_below || item_below == MapType_KEPT_FREE) )
                {
                    map[item_idx] = MapType_BLOCK_WEST;
                    --puzzle_count_to_place;
                    *placed_puzzle_count_ref += 2;
                    map[item_idx-10] = MapType_KEPT_FREE;
                    map[item_idx+10] = MapType_KEPT_FREE;
                    map[item_idx-1] = MapType_PUZZLE_CANDIDATE;
                    map[item_idx-11] = MapType_KEPT_FREE;
                    map[item_idx+9] = MapType_KEPT_FREE;
                    ++*placed_blockade_count_ref;
                } else if (distance_is_less_than_iteration && item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                    map[item_idx] = MapType_EMPTY;
                    
                    ++*placed_puzzle_count_ref;
                    --puzzle_count_to_place;
                }
            } else if ( item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                
                map[item_idx] = MapType_EMPTY;
                
                ++*placed_puzzle_count_ref;
                --puzzle_count_to_place;
            }
        } else if ( item_above != MapType_NONE && item_above != MapType_KEPT_FREE ) {  // Look at neighbor above
            last_item = map[item_idx-10];
            switch (item_above) {
                case MapType_PUZZLE_CANDIDATE:
                case MapType_BLOCK_SOUTH:
                    if ((item_before == MapType_NONE || item_before == MapType_KEPT_FREE)
                        && (item_after == MapType_NONE || item_after == MapType_KEPT_FREE) ) {
                        map[item_idx] = MapType_PUZZLE_CANDIDATE;
                        map[item_idx-1] = MapType_KEPT_FREE;
                        map[item_idx+1] = MapType_KEPT_FREE;
                        
                        ++*placed_puzzle_count_ref;
                        --puzzle_count_to_place;
                    }
                    /* INTENTIONAL FALLTHROUGH */
                case MapType_NONE:
                case MapType_BLOCK_WEST:
                case MapType_INVALID:
                case MapType_TRAVEL_END:
                case MapType_KEPT_FREE:
                case MapType_ISLAND:
                case MapType_BLOCK_EAST:
                case MapType_BLOCK_NORTH:
                    tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
                    continue;
                case MapType_EMPTY:
                case MapType_TRAVEL_START:
                case MapType_SPACEPORT:
                case MapType_PUZZLE:
                    break;
            }
            
            if ( blockade_count > *placed_blockade_count_ref && win_rand() % probablility < threshold ) {
                int distance_less_than_iteration = MapGenerator::GetDistanceToCenter(x, y - 1) < iteration;
                
                if (distance_less_than_iteration && ( !item_before || item_before == MapType_KEPT_FREE )
                    && ( !item_after || item_after == MapType_KEPT_FREE )) {
                    map[item_idx] = MapType_BLOCK_SOUTH;
                    --puzzle_count_to_place;
                    *placed_puzzle_count_ref += 2;
                    map[item_idx-1] = MapType_KEPT_FREE;
                    map[item_idx+1] = MapType_KEPT_FREE;
                    map[item_idx+10] = MapType_PUZZLE_CANDIDATE;
                    map[item_idx+9] = MapType_KEPT_FREE;
                    map[item_idx+11] = MapType_KEPT_FREE;
                    ++*placed_blockade_count_ref;
                } else if (distance_less_than_iteration &&  item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                    map[item_idx] = MapType_EMPTY;
                    
                    ++*placed_puzzle_count_ref;
                    --puzzle_count_to_place;
                }
            } else if ( item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                map[item_idx] = MapType_EMPTY;
                
                ++*placed_puzzle_count_ref;
                --puzzle_count_to_place;
            }
        } else { // Look at neighbor below
            last_item = map[item_idx+10];
            switch ( item_below ) {
                case MapType_PUZZLE_CANDIDATE:
                case MapType_BLOCK_NORTH:
                    if((item_before == MapType_NONE || item_before == MapType_KEPT_FREE)
                       && (item_after == MapType_NONE || item_after == MapType_KEPT_FREE)) {
                        map[item_idx] = MapType_PUZZLE_CANDIDATE;
                        map[item_idx-1] = MapType_KEPT_FREE;
                        map[item_idx+1] = MapType_KEPT_FREE;
                        
                        ++*placed_puzzle_count_ref;
                        --puzzle_count_to_place;
                    }
                    /* INTENTIONAL FALLTHROUGH */
                case MapType_NONE:
                case MapType_BLOCK_EAST:
                case MapType_BLOCK_SOUTH:
                case MapType_ISLAND:
                case MapType_KEPT_FREE:
                case MapType_TRAVEL_END:
                case MapType_INVALID:
                case MapType_BLOCK_WEST:
                    tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
                    continue;
                case MapType_SPACEPORT:
                case MapType_PUZZLE:
                case MapType_TRAVEL_START:
                case MapType_EMPTY:
                    break;
            }
            
            if ( blockade_count > *placed_blockade_count_ref && win_rand() % probablility < threshold ) {
                int distance_less_than_iteration = MapGenerator::GetDistanceToCenter(x, y + 1) < iteration;
                
                if (distance_less_than_iteration && ( !item_before || item_before == MapType_KEPT_FREE )
                    && ( !item_after || item_after == MapType_KEPT_FREE ))
                {
                    map[item_idx] = MapType_BLOCK_NORTH;
                    --puzzle_count_to_place;
                    *placed_puzzle_count_ref += 2;
                    map[item_idx-1] = MapType_KEPT_FREE;
                    map[item_idx+1] = MapType_KEPT_FREE;
                    map[item_idx-10] = MapType_PUZZLE_CANDIDATE;
                    map[item_idx-11] = MapType_KEPT_FREE;
                    map[item_idx-9] = MapType_KEPT_FREE;
                    ++*placed_blockade_count_ref;
                } else if ( distance_less_than_iteration && item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE) {
                    map[item_idx] = MapType_EMPTY;
                    ++*placed_puzzle_count_ref;
                    --puzzle_count_to_place;
                }
            } else if ( item_before < MapType_PUZZLE_CANDIDATE && item_after < MapType_PUZZLE_CANDIDATE && item_above < MapType_PUZZLE_CANDIDATE && item_below < MapType_PUZZLE_CANDIDATE ) {
                map[item_idx] = MapType_EMPTY;
                ++*placed_puzzle_count_ref;
                --puzzle_count_to_place;
            }
        }
        
        tryPlacingTransport(item_idx, transport_count, placed_transport_count_ref, iteration, threshold_2, last_item);
    }
    
    return *placed_blockade_count_ref;
}

int MapGenerator::tryPlacingTransport(int item_idx, int transport_count, int *placed_transport_count_ref, int iteration, int threshold_2, int v36) {
    // Message("try_placing_transport\n");

    if (map[item_idx] == MapType_EMPTY && transport_count > *placed_transport_count_ref
        && ((win_rand()) & 7) < threshold_2 && v36 != MapType_TRAVEL_START && iteration > 2 )
    {
        map[item_idx] = MapType_TRAVEL_START;
        // Message("yep\n");
        ++*placed_transport_count_ref;
    }
    return 0;
}

#pragma mark -
int MapGenerator::additionalPuzzleLocations(int travels_to_place, int* placed_puzzles_count_ref){
    Message("AdditionalPuzzleLocations(%d)\n", travels_to_place);
    for(int i=0; i <= 400 && travels_to_place > 0; i++) {
        int x,y;
        int is_vertical = win_rand() % 2;
        if (is_vertical) {
            x = win_rand() % 2 ? 0 : 9;
            y = win_rand() % 10;
        } else {
            y = win_rand() % 2 ? 0 : 9;
            x = win_rand() % 10;
        }
        
        int world_idx = x + 10 * y;
        if (map[world_idx] != 0)
            continue;
        
        int16 item_before = getFromIgnoringBounds(x-1, y, (int16*)map);
        int16 item_after = getFromIgnoringBounds(x+1, y, (int16*)map);
        int16 item_above = getFromIgnoringBounds(x, y-1, (int16*)map);
        int16 item_below = getFromIgnoringBounds(x, y+1, (int16*)map);
        
        int y_diff = 0, x_diff = 0;
        if (is_vertical && x == 9 && item_before != MapType_KEPT_FREE) {
            x_diff = 1;
            y_diff = 0;
        } else if(is_vertical && x == 0 && item_after != MapType_KEPT_FREE) {
            x_diff = -1;
            y_diff = 0;
        } else if(!is_vertical && y == 9 && item_above != MapType_KEPT_FREE) {
            x_diff = 0;
            y_diff = 1;
        } else if(!is_vertical && y == 0 && item_below != MapType_KEPT_FREE) {
            x_diff = 0;
            y_diff = -1;
        }
        
        if(x_diff == 0 && y_diff == 0) continue;
        
        int16 item_neighbor = get(x-x_diff, y - y_diff);
        if(item_neighbor <= MapType_NONE) continue;
        
        switch (item_neighbor) {
            case MapType_EMPTY:
            case MapType_TRAVEL_START:
            case MapType_SPACEPORT:
                map[world_idx] = 1;
                break;
            case MapType_PUZZLE_CANDIDATE:
                map[world_idx] = 300;
                
                if (!x_diff ) {
                    if (x > 0) map[world_idx - 1] = MapType_KEPT_FREE;
                    if (x < 9) map[world_idx + 1] = MapType_KEPT_FREE;
                } else if (!y_diff) {
                    if ( y > 0 ) map[world_idx - 10] = MapType_KEPT_FREE;
                    if ( y < 9 ) map[world_idx + 10] = MapType_KEPT_FREE;
                }
                
                continue;
            case MapType_BLOCK_WEST:
                if (x_diff != -1) continue;
                if (MapType_NONE < item_above && item_above <= MapType_BLOCK_NORTH)
                    continue;
                if (MapType_NONE < item_below && item_below <= MapType_BLOCK_NORTH)
                    continue;
                
                map[world_idx] = 300;
                
                if ( y > 0 ) map[world_idx - 10] = MapType_KEPT_FREE;
                if ( y < 9 ) map[world_idx + 10] = MapType_KEPT_FREE;
                break;
            case MapType_BLOCK_EAST:
                if ( x_diff != 1 ) continue;
                if (MapType_NONE < item_below && item_below <= MapType_BLOCK_NORTH) continue;
                if (MapType_NONE < item_above && item_above <= MapType_BLOCK_NORTH) continue;
                
                map[world_idx] = 300;
                
                if ( y > 0 ) map[world_idx - 10] = MapType_KEPT_FREE;
                if ( y < 9 ) map[world_idx + 10] = MapType_KEPT_FREE;
                break;
            case MapType_BLOCK_NORTH:
                if ( y_diff != -1 ) continue;
                if ( MapType_NONE < item_before && item_before <= MapType_BLOCK_NORTH) continue;
                if ( MapType_NONE < item_after && item_after <= MapType_BLOCK_NORTH) continue;
                
                map[world_idx] = 300;
                
                if ( x > 0 ) map[world_idx-1] = MapType_KEPT_FREE;
                if ( x < 9 ) map[world_idx + 1] = MapType_KEPT_FREE;
                break;
            case MapType_BLOCK_SOUTH:
                if ( y_diff != 1 ) continue;
                if (MapType_NONE < item_before && item_before <= MapType_BLOCK_NORTH) continue;
                if (MapType_NONE < item_after && item_after <= MapType_BLOCK_NORTH) continue;
                
                map[world_idx] = 300;
                
                if ( x > 0)  map[world_idx-1] = MapType_KEPT_FREE;
                if ( x < 9 ) map[world_idx + 1] = MapType_KEPT_FREE;
                break;
            default: continue;
        }

        ++*placed_puzzles_count_ref;
        --travels_to_place;
    }
    
    return 0;
}

#pragma mark -

int MapGenerator::getIslandOrientation(int x, int y)
{
    if(x!=0 && get(x-1, y) == 104) { return 1; }
    if(x!=9 && get(x+1, y) == 104) { return 3; }
    if(y!=0 && get(x, y-1) == 104) { return 2; }
    if(y!=9 && get(x, y+1) == 104) { return 4; }
    
    return -1;
}

int MapGenerator::choosePuzzlesBehindBlockades() {
    int puzzles_placed = 0;
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++){
            int i = x + y * 10;
            switch (get(x, y)) {
                case MapType_BLOCK_WEST:
                    if (get(x-1, y) == 300 ) {
                        if ( x <= 1 || get(x-2, y) != 300 ){
                            map[i-1] = 306;
                            puzzles[i-1] = puzzles_placed;
                        } else {
                            map[i-2] = 306;
                            puzzles[i-2] = puzzles_placed;
                        }
                        puzzles_placed++;
                    }
                    break;
                case MapType_BLOCK_EAST:
                    if (get(x+1, y) == 300 ) {
                        if ( x >= 8 || get(x+2, y) != 300 ) {
                            map[i+1] = 306;
                            puzzles[i+1] = puzzles_placed;
                        } else {
                            map[i+2] = 306;
                            puzzles[i+2] = puzzles_placed;
                        }
                        puzzles_placed++;
                    }
                    break;
                case MapType_BLOCK_NORTH:
                    if(get(x, y-1) == 300) {
                        if ( y <= 1 || get(x, y-2) != 300 ) {
                            set(x, y-1, 306);
                            setPuzzle(x, y-1, puzzles_placed);
                        } else {
                            set(x, y-2, 306);
                            setPuzzle(x, y-2, puzzles_placed);
                        }
                        puzzles_placed++;
                    }
                    break;
                case MapType_BLOCK_SOUTH:
                    if(get(x, y+1) == 300 ) {
                        if ( y >= 8 || get(x, y+2) != 300 ) {
                            set(x, y+1, 306);
                            setPuzzle(x, y+1, puzzles_placed);
                        } else {
                            set(x, y+2, 306);
                            setPuzzle(x, y+2, puzzles_placed);
                        }
                        puzzles_placed++;
                    }
                    break;
                default: break;
            }
        }
    }
    
    return puzzles_placed;
}

int MapGenerator::choosePuzzlesOnIslands(int count) {
    // Message("choose_puzzles_on_islands(%d)\n", count);
    for(int y=0; y < 10; y++) {
        for(int x=0; x < 10; x++) {
            if (get(x, y) == 102 ) {
                switch(getIslandOrientation(x, y)) {
                    case 1:{
                        int puzzle_x = x - 1;
                        int do_break = 0;
                        do {
                            if ( puzzle_x >= 0 ) {
                                if (get(puzzle_x, y) == 104 ) {
                                    --puzzle_x;
                                } else {
                                    ++puzzle_x;
                                    do_break = 1;
                                }
                            } else {
                                puzzle_x = 0;
                                do_break = 1;
                            }
                        } while ( !do_break );
                        
                        set(puzzle_x, y, 306);
                        setPuzzle(puzzle_x, y, count++);
                        
                        break;
                    }
                    case 2: {
                        int puzzle_y = y - 1;
                        int do_break = 0;
                        do {
                            if ( puzzle_y >= 0 ) {
                                if ( get(x, puzzle_y) == 104 ) {
                                    --puzzle_y;
                                } else {
                                    ++puzzle_y;
                                    do_break = 1;
                                }
                            } else {
                                puzzle_y = 0;
                                do_break = 1;
                            }
                        } while ( !do_break );
                        
                        set(x, puzzle_y, 306);
                        setPuzzle(x, puzzle_y, count++);
                        
                        break;
                    }
                    case 3: {
                        int puzzle_x = x + 1;
                        int do_break = 0;
                        do {
                            if ( puzzle_x <= 9 ) {
                                if (get(puzzle_x, y) == 104 ) {
                                    ++puzzle_x;
                                } else {
                                    --puzzle_x;
                                    do_break = 1;
                                }
                            } else {
                                puzzle_x = 9;
                                do_break = 1;
                            }
                        } while ( !do_break );
                        
                        set(puzzle_x, y, 306);
                        setPuzzle(puzzle_x, y, count++);
                        break;
                    }
                    case 4: {
                        int do_break = 0;
                        int puzzle_y = y + 1;
                        do {
                            if ( puzzle_y <= 9 ) {
                                if ( get(x, puzzle_y) == 104 ) {
                                    ++puzzle_y;
                                } else {
                                    --puzzle_y;
                                    ++do_break;
                                }
                            } else {
                                puzzle_y = 9;
                                ++do_break;
                            }
                        } while ( !do_break );
                        
                        set(x, puzzle_y, 306);
                        setPuzzle(x, puzzle_y, count++);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    return count;
}

int MapGenerator::chooseAdditionalPuzzles(int placed_puzzles, int total_puzzle_count) {
    // Message("choose_additional_puzzles(%d, %d)\n", placed_puzzles, total_puzzle_count);
    int i;
    int do_break = 0;
    int j=0;
    for(i=0; i <= 200; i++) {
        j++;
        // Message("%d >= %d\n", placed_puzzles, total_puzzle_count);
        if(placed_puzzles >= total_puzzle_count) {
            // Message("inc something 1\n");
            do_break = 1;
        }

        // Message("%d > 200\n", i);
        int x,y;
        
        if(i > 200) {
            // Message("inc something 2\n");
            do_break = 1;
        }
        if(placed_puzzles >= total_puzzle_count) do_break = 1;
        
        x = win_rand() % 10;
        if ( i >= 50 || x == 0 || x == 9 ) {
            y = win_rand() % 10;
        } else {
            y = (win_rand() & 1) < 1 ? 9 : 0;
        }
        
        // asm compares something against 400, 150 and maybe blockade_count
        if (placed_puzzles >= total_puzzle_count)
            break;
        
        int distance = MapGenerator::GetDistanceToCenter(x, y);
        int didPlace = false;
        if (distance >= 3 || i >= 150)
        {
            uint16_t item = get(x, y);
            if ( (item == 1 || item == 300)
                && ( x == 0 || get(x-1, y) != 306 )
                && ( x == 9 || get(x+1, y) != 306 )
                && ( y == 0 || get(x, y-1) != 306 )
                && ( y == 9 || get(x, y+1) != 306 )) {
                
                set(x, y, 306);
                setPuzzle(x, y, placed_puzzles++);
                // Message("did_place %d => %d\n", placed_puzzles-1, placed_puzzles);
                didPlace = true;
            }
            
            if (placed_puzzles >= total_puzzle_count)
                break;
        }

        if(distance < 3 && i < 150) i--;
        else {
            // Message("%d => %d\n", i, i+1);
        }
        if(do_break) break;
    }
    
    return placed_puzzles;
}

int MapGenerator::countStuff(int* travels, int* blockades, int *puzzles){
    *travels = 0, *blockades = 0, *puzzles = 0;
    for(int i=0; i < 100; i++){
        switch (map[i]) {
            case MapType_EMPTY:
            case MapType_ISLAND:
            case MapType_PUZZLE_CANDIDATE:
                *puzzles += 1;
                break;
            case MapType_BLOCK_NORTH:
            case MapType_BLOCK_EAST:
            case MapType_BLOCK_SOUTH:
            case MapType_BLOCK_WEST:
                *blockades += 1;
                break;
            case MapType_TRAVEL_START:
                *travels += 1;
            default:
                break;
        }
    }
    return 0;
}

int MapGenerator::makeSureLastPuzzleIsNotTooCloseToCenter(int placed_puzzles) {
    // Message("make_sure_last_puzzle_is_not_too_close_to_center(%d)\n", placed_puzzles);
    int max_puzzle_x = 0, max_puzzle_y = 0;
    findPuzzle(placed_puzzles-1, &max_puzzle_x, &max_puzzle_y);
    
    if(MapGenerator::GetDistanceToCenter(max_puzzle_x, max_puzzle_y) < 3) {
        for(int y=0; y < 10; y++) {
            for(int x=0; x < 10; x++) {
                if(getPuzzle(x, y) >= 0
                   && MapGenerator::GetDistanceToCenter(x, y) >= 3
                   && (x != max_puzzle_x || y != max_puzzle_y)) {
                    int replacement_puzzle = getPuzzle(x, y);
                    setPuzzle(max_puzzle_x, max_puzzle_y, replacement_puzzle);
                    setPuzzle(x, y, placed_puzzles-1);
                    
                    return 0;
                }
            }
        }
    }
    return 0;
}

int MapGenerator::findPuzzle(int count, int *x, int *y){
    for(int max_puzzle_y = 0; max_puzzle_y < 10; max_puzzle_y++){
        for(int max_puzzle_x = 0; max_puzzle_x < 10; max_puzzle_x++){
            if(getPuzzle(max_puzzle_x, max_puzzle_y) == count) {
                *x = max_puzzle_x;
                *y = max_puzzle_y;
                return 0;
            }
        }
    }
    return 0;
}

int MapGenerator::placeSomethingOnTheMap() {
    Message("place_intermediate_world_thing()\n");
    int placed_puzzles = 0;
    int travel_count = 0, puzzles_count = 0, blockades_count = 0;;
    countStuff(&travel_count, &blockades_count, &puzzles_count);
    
    int total_puzzle_count = puzzles_count / 4 + win_rand() % (puzzles_count / 5 + 1) - blockades_count - travel_count - 2;
    if ( total_puzzle_count < 4 ) total_puzzle_count = 4;
    
    placed_puzzles = choosePuzzlesBehindBlockades();
    placed_puzzles = choosePuzzlesOnIslands(placed_puzzles);
    placed_puzzles = chooseAdditionalPuzzles(placed_puzzles, total_puzzle_count);
    
    makeSureLastPuzzleIsNotTooCloseToCenter(placed_puzzles);
    
    return placed_puzzles;
}

#pragma mark - Transports
void MapGenerator::placeTransports(int number_of_transports)
{
    Message("MapGenerator::PlaceTransports(%d)\n", number_of_transports);
    if(number_of_transports <= 0) return;
    
    for(int i=0; i < number_of_transports; i++) {
        placeTransport();
    }
}

void MapGenerator::placeTransport() {
    for(int i=0; i <= 200; i++) {
        switch (win_rand() % 4) {
            case 0: if(placeTransportLeft()) return; break;
            case 1: if(placeTransportTop()) return; break;
            case 2: if(placeTransportBottom()) return; break;
            case 3: if(placeTransportRight()) return; break;
            default: break;
        }
    }
}

int MapGenerator::placeTransportLeft(){
    int v51 = 0;
    int v3 = 0;
    int v4 = 0;
    int v5 = 0;
    int idx = 0;
    for(int i=0; i < 10; i++) {
        if (map[idx] || (map[idx+1] && map[idx+1] != MapType_KEPT_FREE)) {
            if ( v4 < v3 ) {
                v4 = v3;
                v5 = i - v3;
            }
            v3 = 0;
        } else {
            ++v3;
        }
        idx += 10;
    }
    
    if ( v4 < v3 ) {
        v4 = v3;
        v5 = 10 - v3;
    }
    if ( v4 < 3 ) return 0;
    if ( v4 == 3 && v5 > 0 && v5 < 7 ) return 0;
    
    if ( v4 == 3 && (!v5 || v5 == 7) ) v51 = 2;
    if ( v4 >= 4 ) v51 = v4 - 2;
    if ( v51 > 4 ) v51 = 4;
    if ( v5 > 0 && v5 + v51 <= 9 )  ++v5;
    if ( win_rand() % 2 )
    {
        map[10 * v5] = 102;
        if ( v5 + 1 < v5 + v51 )
        {
            int v2 = v51 - 1;
            int another_idx = 10 * (v5 + 1);
            do {
                map[another_idx] = 104;
                another_idx += 10;
                --v2;
            }
            while ( v2 );
        }
    }
    else
    {
        int v100 = v5 + v51 - 1;
        map[5 * (2 * v100 + 2) - 10] = 102;
        if ( v5 < v100 ) {
            int another_idx = 10 * v5;
            int v2 = v51 - 1;
            do {
                map[another_idx] = 104;
                another_idx += 10;
                --v2;
            }
            while ( v2 );
        }
    }
    return 1;
}

int MapGenerator::placeTransportTop(){
    int v2;
    unsigned int v23;
    
    int v52 = 0;
    int i;
    
    int v12 = 0;
    int v13 = 0;
    int v14 = 0;
    int idx = 0;
    for(int i=0; i < 10; i++) {
        if (map[idx] || (map[idx + 10] && map[idx + 10] != 305)) {
            if ( v13 < v12 ) {
                v13 = v12;
                v14 = i - v12;
            }
            v12 = 0;
        } else {
            ++v12;
        }
        idx ++;
    }
    
    if ( v13 < v12 ) {
        v13 = v12;
        v14 = 10 - v12;
    }
    
    if ( v13 < 3 ) return 0;
    if ( v13 == 3 && v14 > 0 && v14 < 7 ) return 0;
    
    if ( v13 == 3 && (!v14 || v14 == 7) ) v52 = 2;
    
    if ( v13 >= 4 ) v52 = v13 - 2;
    if ( v52 > 4 ) v52 = 4;
    if ( v14 > 0 && v14 + v52 <= 9 ) ++v14;
    
    if ( win_rand() % 2 ) {
        map[v14] = 102;
        if ( v14 + 1 < v14 + v52 ) {
            for ( i = v14+1; i < v14 + v52; i++ ) {
                map[i] = 104;
            }
        }
    } else {
        map[v14 - 1 + v52] = 102;
        v2 = v14 + v52 - 1;
        if ( v14 < v2 ) {
            v23 = (v52 - 1) >> 1;
            if(v23) map[v14] = 0x68;
            for(int j=v14; j < v14 + v52-1; j++) {
                map[j] = 104;
            }
        }
    }
    return 1;
}

int MapGenerator::placeTransportBottom()
{
    int v2;
    int v53 = 0;
    int v26 = 0;
    int v27 = 0;
    int v28 = 0;
    
    int idx = 90;
    for(int i=0; i < 10; i++) {
        if (map[idx] || (map[idx-10] && map[idx-10] != 305)) {
            if ( v27 < v26 ) {
                v27 = v26;
                v28 = i - v26;
            }
            v26 = 0;
        } else {
            ++v26;
        }
        idx++;
    }
    
    if ( v27 < v26 )
    {
        v27 = v26;
        v28 = 10 - v26;
    }
    
    if(v27 < 4) return 0;
    if ( v27 == 3 && v28 > 0 && v28 < 7 ) return 0;
    
    if (v27 == 3 && (!v28 || v28 == 7)) v53 = 2;
    if ( v27 >= 4 ) v53 = v27 - 2;
    if ( v53 > 4 ) v53 = 4;
    if ( v28 > 0 && v28 + v53 <= 9 ) ++v28;
    
    if ( win_rand() % 2 )
    {
        map[v28 + 90] = 102;
        for(int i=v28 + 91; i < v28 + 90 + v53; i++){
            map[i] = 104;
        }
    }
    else
    {
        map[v28 + 89  + v53] = 102;
        v2 = v28 + v53 - 1;
        if ( v28 < v2 )
        {
            for(int i=v28 + 90; i < v28 + 89+v53; i++) {
                map[i] = 104;
            }
        }
    }
    return 1;
}

int MapGenerator::placeTransportRight()
{
    int v54 = 0;
    int y;
    int v2 = 0;
    
    int v40 = 0;
    int v41 = 0;
    y = 0;
    int idx = 9;
    for(int i=0; i < 10; i++) {
        if (map[idx] || (map[idx-1] && map[idx-1] != 305)) {
            if ( v41 < v40 ) {
                v41 = v40;
                y = i - v40;
            }
            v40 = 0;
        } else {
            ++v40;
        }
        idx += 10;
    }
    
    if ( v41 < v40 ) {
        v41 = v40;
        y = 10 - v40;
    }
    
    if(v41 < 4) return 0;
    if ( v41 == 3 && y > 0 && y < 7 ) return 0;
    
    if ( v41 == 3 && (!y || y == 7 )) v54 = 2;
    if ( v41 >= 4 ) v54 = v41 - 2;
    if ( v54 > 4 ) v54 = 4;
    if ( y > 0 && y + v54 <= 9 ) ++y;

    if ( win_rand() % 2 ) {
        map[10 * y + 9] = 102;
        for(int i=1; i < v54; i++) {
            map[10 * (y + i) + 9] = 104;
        }
        return 1;
    } else {
        // up
        v2 = y + v54 - 1;
        map[10 * (y+v54-1) + 9] = 102;
        for(int i=1; i < v54; i++) {
            map[10 * (y+v54-1-i) + 9] = 104;
        }
        return 1;
    }

    return 1;
}

int16 MapGenerator::getFrom(int x, int y, int16 *map) {
    if(x < 0 || y < 0 || x >= 10 || y >= 10) {
        printf("out of bounds\n");
        return -1;
    }

    return map[x+10*y];
}

int16 MapGenerator::getFromIgnoringBounds(int x, int y, int16 *map) {
    if(x < 0 || y < 0 || x >= 10 || y >= 10) {
        return -1;
    }

    return map[x+10*y];
}

void MapGenerator::setTo(int x, int y, int16 value, int16 *map) {
    if(x < 0 || y < 0 || x >= 10 || y >= 10) {
        printf("out of bounds\n");
    }

    map[x+10*y] = value;
}
