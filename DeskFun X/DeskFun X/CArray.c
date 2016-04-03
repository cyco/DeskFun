//
//  CArray.c
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "CArray.h"
#include <stdlib.h>

typedef struct  {
    int count;
    void *ptrs;
} CArray;

CArray* CArrayCArray(CArray*this) {
    this->count = 0;
    this->ptrs = malloc(0);
    return this;
}

CArray* CArrayDestruct(CArray*this) {
    this->count = 0;
    free(this->ptrs);
    
    return NULL;
}