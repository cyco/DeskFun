//
//  YodaDocument.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "YodaDocument.hpp"

YodaDocument::YodaDocument(){
    puzzles_can_be_reused = -1;
}

void YodaDocument::ShuffleVector(vector<int16> &array) {
    int16 v2; // di@1
    int v3; // eax@4
    int v7; // edi@10
    signed int v8; // ebx@11
    int16 v12; // dx@22
    int v13; // eax@24
    vector<int16> temp_array; // [sp+Ch] [bp-2Ch]@2
    int v16; // [sp+20h] [bp-18h]@2
    int16 v17; // [sp+26h] [bp-12h]@11
    int16 v18; // [sp+28h] [bp-10h]@9
    int16 count; // [sp+2Ah] [bp-Eh]@1
    
    v2 = 0;
    count = array.size();
    if ( count > 0 )
    {
        v16 = count;
        temp_array.resize(count, -1);
        while ( count > v2 )
        {
            v3 = v2++;
            temp_array[v3] = -1;
        }
        int v4 = 0;
        if ( count > 0 )
        {
            do
            {
                int idx = win_rand() % v16;
                if ( temp_array[idx] == -1 )
                {
                    int v6 = v4;
                    temp_array[idx] = array[v6];
                    array[v6] = -1;
                }
                ++v4;
            }
            while ( count > v4 );
        }
        v18 = count - 1;
        if ( (count - 1) >= 0 )
        {
            do
            {
                v7 = 2 * v18;
                if (array[7] != -1 )
                {
                    v8 = 0;
                    v17 = 0;
                    do
                    {
                        if ( count > 0 )
                        {
                            int v9 = 0;
                            int v10 = v16;
                            do
                            {
                                if ( temp_array[v9] == -1 )
                                    v8 = 1;
                                v9++;
                                --v10;
                            }
                            while ( v10 );
                        }
                        if ( !v8 )
                            break;
                        
                        int idx = win_rand() % v16;
                        
                        if ( temp_array[idx] == -1 )
                        {
                            ++v17;
                            temp_array[idx] = array[v7];
                            array[v7] = -1;
                        }
                    }
                    while ( !v17 );
                }
                --v18;
            }
            while ( v18 >= 0 );
        }
        v12 = 0;
        while ( count > v12 )
        {
            v13 = v12++;
            array[v13] = temp_array[v13];
        }
    }
}

int16 YodaDocument::GetNewPuzzleId(__uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    int16 puzzle_id; // dx@57
    Puzzle *puzzle_1; // ebx@57
    int break_from_loop = 0;
    
    vector<int16> puzzle_ids;
    GetPuzzleCandidates(puzzle_ids, item_id, a3, zone_type, a5);
    if (puzzle_ids.size() <= 0 ) {
        return -1;
    }
    
    ShuffleVector(puzzle_ids);
    
    size_t count = puzzle_ids.size();
    int16 puzzle_idx = 0;
    while ( 1 )
    {
        puzzle_id = puzzle_ids[puzzle_idx];
        puzzle_1 = this->puzzles[puzzle_id];
        if ( ContainsPuzzleId(puzzle_id) )
            goto LABEL_75;
        if ( zone_type <= (signed int)ZONETYPE_Trade )
        {
            if ( zone_type == ZONETYPE_Trade )
            {
                if ( puzzle_1->type == PuzzleTypeTrade && puzzle_1->questItemIDs[0] == item_id )
                {
                    return puzzle_ids[puzzle_idx];
                }
            }
            else if ( zone_type == ZONETYPE_Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->questItemIDs[0] == item_id )
            {
                return puzzle_ids[puzzle_idx];
            }
            goto LABEL_75;
        }
        if ( zone_type != ZONETYPE_Use )
            break;
        if ( puzzle_1->type == PuzzleTypeU1 && puzzle_1->questItemIDs[0] == item_id )
        {
            return puzzle_ids[puzzle_idx];
        }
    LABEL_75:
        if ( !break_from_loop )
        {
            ++puzzle_idx;
            if ( puzzle_idx >= count )
                ++break_from_loop;
            if ( !break_from_loop )
                continue;
        }
        return -1;
    }
    if ( zone_type != 9999 || puzzle_1->type != PuzzleTypeEnd )
        goto LABEL_75;

    return puzzle_ids[puzzle_idx];
}

void YodaDocument::GetPuzzleCandidates(vector<int16> &result, __uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5) {
    Puzzle *puzzle; // edx@2
    __int16 count_3; // [sp+2Ah] [bp-16h]@1
    int16 puzzle_idx; // [sp+2Ch] [bp-14h]@1
    
    count_3 = this->puzzles.size();
    result.clear();
    
    puzzle_idx = 0;
    if ( count_3 > 0 )
    {
        while ( 1 )
        {
            puzzle = this->puzzles[puzzle_idx];
            if ( (int16)zone_type <= (signed int)ZONETYPE_Trade )
                break;
            if ( zone_type == ZONETYPE_Use )
            {
                if ( puzzle->type || ContainsPuzzleId(puzzle_idx) )
                    goto do_break;
            use_puzzle:
                printf("Add puzzle 0x%0x at %lu\n", puzzle_idx, result.size());
                result.insert(result.end(), puzzle_idx);
                goto do_break;
            }
            if ( zone_type != 9999 || puzzle->type != PuzzleTypeEnd )
                goto do_break;
            
            if ( PuzzleUsedInLastGame(puzzle_idx, this->planet) && this->puzzles_can_be_reused < 0 )
                goto do_break;
            
            if(PuzzleIsGoal(puzzle_idx, this->planet))
                goto use_puzzle;
            goto do_break;
        
        do_break:
            if ( count_3 <= (int16)++puzzle_idx )
                goto out_of_loop;
        }
        if ( zone_type == ZONETYPE_Trade )
        {
            if ( puzzle->type != PuzzleTypeTrade || ContainsPuzzleId(puzzle_idx) )
                goto do_break;
        }
        else if ( zone_type != ZONETYPE_Goal
                 || puzzle->type != PuzzleTypeU3
                 || ContainsPuzzleId(puzzle_idx) )
        {
            goto do_break;
        }
        goto use_puzzle;
    }
out_of_loop:
    return;
}

int YodaDocument::ContainsPuzzleId(uint16 puzzle_id)
{
    for(uint16 id : chosen_puzzle_ids) {
        if(id == puzzle_id) return 1;
    }
    return 0;
}

int YodaDocument::PuzzleUsedInLastGame(uint16 puzzle_id, Planet planet) {
    return 0;
}

int YodaDocument::PuzzleIsGoal(uint16 puzzle_id, Planet planet){
    if ( planet == TATOOINE )
    {
        switch ( puzzle_id )
        {
            case GOAL_FALCON:
            case GOAL_HAN:
            case GOAL_AMULET:
            case GOAL_ADEGAN_CRYSTAL:
            case GOAL_THREEPIOS_PARTS:
                return 1;
            default:
                return 0;
        }
    }
    
    if ( planet == HOTH )
    {
        switch ( puzzle_id )
        {
            case GOAL_GENERAL_MARUTZ:
            case GOAL_HIDDEN_FACTORY:
            case GOAL_WARN_THE_REBELS:
            case GOAL_RESCUE_YODA:
            case GOAL_CAR:
                return 1;
            default:
                return 0;
        }
    }
    
    if ( planet == ENDOR
        && puzzle_id >= (signed int)GOAL_FIND_LEIA
        && (puzzle_id <= (signed int)GOAL_IMPERIAL_BATTLE_STATION
            || puzzle_id == GOAL_LANTERN_OF_SACRED_LIGHT) )
    {
        return 1;
    }
    
    return 0;
}