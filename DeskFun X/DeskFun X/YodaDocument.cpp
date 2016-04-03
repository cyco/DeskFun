//
//  YodaDocument.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "YodaDocument.hpp"

int YodaDocument::GetNewPuzzleId(__uint16_t item_id, int a3, ZONE_TYPE zone_type, int a5)
{
    /*
    int v5; // ebp@0
    YodaDocument *document; // esi@1
    YPuzzle *puzzle; // edx@2
    Planet planet; // edx@19
    int count_2; // eax@23
    __uint16_t *puzzles; // edi@24
    int count_1; // edi@29
    __uint16_t *offset_1; // eax@30
    int count; // edi@35
    __uint16_t *offset_2; // eax@36
    __uint16_t puzzle_id; // dx@57
    YPuzzle *puzzle_1; // ebx@57
    int v18; // ecx@80
    CWordArray *puzzle_ids = new CWordArray(); // [sp+10h] [bp-30h]@1
    int puzzle_was_used_in_last_game; // [sp+24h] [bp-1Ch]@19
    unsigned int dbl_idx; // [sp+28h] [bp-18h]@57
    __uint16_t count_3; // [sp+2Ah] [bp-16h]@1
    __uint16_t puzzle_idxs[4]; // [sp+2Ch] [bp-14h]@1
    int v24; // [sp+3Ch] [bp-4h]@1

    puzzle_idxs[3] = 0;
    count_3 = this->puzzles.count;
    v24 = 0;
    puzzle_idxs[1] = 0;
    
    if ( count_3 > 0 )
    {
        while ( 1 )
        {
            puzzle = this->puzzles.ptrs[puzzle_idxs[1]];
            if ( zone_type <= ZONETYPE_Trade )
                break;
            if ( zone_type == ZONETYPE_Use )
            {
                if ( puzzle->type || ContainsPuzzleId(document, puzzle_idxs[1]) )
                    goto do_break;
            use_puzzle:
                CWordArray::SetAtGrow(&puzzle_ids, puzzle_ids->count, puzzle_idxs[1]);
                goto do_break;
            }
            if ( zone_type != 9999 || puzzle->type != PuzzleTypeEnd )
                goto do_break;
            puzzle_was_used_in_last_game = 0;
            planet = this->planet;
            switch ( planet )
            {
                case TATOOINE:
                    count_2 = this->tatooine_puzzle_ids.count;
                    if ( count_2 > 0 )
                    {
                        puzzles = this->tatooine_puzzle_ids.ptrs;
                        do
                        {
                            if ( (unsigned __uint16_t)*puzzles == puzzle_idxs[1] )
                                puzzle_was_used_in_last_game = 1;
                            ++puzzles;
                            --count_2;
                        }
                        while ( count_2 );
                    }
                    break;
                case HOTH:
                    count_1 = this->hoth_puzzle_ids.count;
                    if ( count_1 > 0 )
                    {
                        offset_1 = this->hoth_puzzle_ids.ptrs;
                        do
                        {
                            if ( (unsigned __uint16_t)*offset_1 == puzzle_idxs[1] )
                                puzzle_was_used_in_last_game = 1;
                            ++offset_1;
                            --count_1;
                        }
                        while ( count_1 );
                    }
                    break;
                case ENDOR:
                    count = this->endor_puzzle_ids.count;
                    if ( count > 0 )
                    {
                        offset_2 = this->endor_puzzle_ids.ptrs;
                        do
                        {
                            if ( (unsigned __uint16_t)*offset_2 == puzzle_idxs[1] )
                                puzzle_was_used_in_last_game = 1;
                            ++offset_2;
                            --count;
                        }
                        while ( count );
                    }
                    break;
            }
            if ( puzzle_was_used_in_last_game && this->contols_puzzle_reuse? < 0 )
                goto do_break;
            if ( planet == TATOOINE )
            {
                switch ( puzzle_idxs[1] )
                {
                    case GOAL_FALCON:
                    case GOAL_HAN:
                    case GOAL_AMULET:
                    case GOAL_ADEGAN_CRYSTAL:
                    case GOAL_THREEPIOS_PARTS:
                        goto use_puzzle;
                    default:
                        goto do_break;
                }
                goto do_break;
            }
            if ( planet == HOTH )
            {
                switch ( puzzle_idxs[1] )
                {
                    case GOAL_GENERAL_MARUTZ:
                    case GOAL_HIDDEN_FACTORY:
                    case GOAL_WARN_THE_REBELS:
                    case GOAL_RESCUE_YODA:
                    case GOAL_CAR:
                        goto use_puzzle;
                    default:
                        goto do_break;
                }
                goto do_break;
            }
            if ( planet == ENDOR
                && puzzle_idxs[1] >= (signed int)GOAL_FIND_LEIA
                && (puzzle_idxs[1] <= (signed int)GOAL_IMPERIAL_BATTLE_STATION
                    || puzzle_idxs[1] == GOAL_LANTERN_OF_SACRED_LIGHT) )
            {
                goto use_puzzle;
            }
        do_break:
            if ( count_3 <= ++puzzle_idxs[1] )
                goto out_of_loop;
        }
        if ( zone_type == Trade )
        {
            if ( puzzle->type != PuzzleTypeTrade || ContainsPuzzleId(document, puzzle_idxs[1]) )
                goto do_break;
        }
        else if ( zone_type != Goal
                 || puzzle->type != PuzzleTypeU3
                 || ContainsPuzzleId(document, puzzle_idxs[1]) )
        {
            goto do_break;
        }
        goto use_puzzle;
    }
out_of_loop:
    if ( puzzle_ids.count <= 0 )
    {
    return_failure:
        v24 = -1;
        deallocate_Array(v5);
        return -1;
    }
    Array::Shuffle((Array *)&puzzle_ids);
    puzzle_idxs[1] = 0;
    puzzle_was_used_in_last_game = puzzle_ids.count;
    while ( 1 )
    {
        puzzle_id = puzzle_ids.ptrs[puzzle_idxs[1]];
        dbl_idx = 2 * puzzle_idxs[1];
        puzzle_1 = this->puzzles.ptrs[(unsigned __uint16_t)puzzle_id];
        if ( ContainsPuzzleId(document, (unsigned __uint16_t)puzzle_id) )
            goto LABEL_75;
        if ( zone_type <= (signed int)Trade )
        {
            if ( zone_type == Trade )
            {
                if ( puzzle_1->type == PuzzleTypeTrade && puzzle_1->item_1 == item_id )
                {
                    *(__uint32_t*)puzzle_idxs = 0;
                    v24 = -1;
                    *(__uint32_t*)puzzle_idxs = puzzle_ids.ptrs[dbl_idx / 2];
                    deallocate_Array(v5);
                    return *(__uint32_t*)puzzle_idxs;
                }
            }
            else if ( zone_type == Goal && puzzle_1->type == PuzzleTypeU3 && puzzle_1->item_1 == item_id )
            {
                *(__uint32_t*)puzzle_idxs = 0;
                v24 = -1;
                *(__uint32_t*)puzzle_idxs = puzzle_ids.ptrs[dbl_idx / 2];
                deallocate_Array(v5);
                return *(__uint32_t*)puzzle_idxs;
            }
            goto LABEL_75;
        }
        if ( zone_type != Use )
            break;
        if ( puzzle_1->type == PuzzleTypeU1 && puzzle_1->item_1 == item_id )
        {
            *(__uint32_t*)puzzle_idxs = 0;
            v24 = -1;
            *(__uint32_t*)puzzle_idxs = puzzle_ids.ptrs[dbl_idx / 2];
            deallocate_Array(v5);
            return *(__uint32_t*)puzzle_idxs;
        }
    LABEL_75:
        if ( !puzzle_idxs[3] )
        {
            ++puzzle_idxs[1];
            if ( puzzle_idxs[1] >= puzzle_was_used_in_last_game )
                ++puzzle_idxs[3];
            if ( !puzzle_idxs[3] )
                continue;
        }
        goto return_failure;
    }
    if ( zone_type != 9999 || puzzle_1->type != PuzzleTypeEnd )
        goto LABEL_75;
    v24 = -1;
    v18 = puzzle_idxs[1];
    *(__uint32_t*)puzzle_idxs = 0;
    *(__uint32_t*)puzzle_idxs = puzzle_ids.ptrs[v18];
    deallocate_Array(v5);
    return *(__uint32_t*)puzzle_idxs;
     //*/
    return 0;
}