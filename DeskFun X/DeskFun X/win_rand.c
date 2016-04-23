//
//  win_rand.c
//  reimp
//
//  Created by Christoph Leimbrock on 26/03/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "win_rand.h"

__thread static unsigned long current_rand = 0;
void win_srand(int seed) {
    current_rand = seed;
}

int win_rand(void){
    return (((current_rand = (int)(current_rand * 214013L)
              + 2531011L) >> 16) & 0x7fff );
}
