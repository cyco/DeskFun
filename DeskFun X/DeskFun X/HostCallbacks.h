//
//  HostCallbacks.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__HostCallbacks__
#define __Desktop_Adventure__HostCallbacks__

#include <stdio.h>

namespace Host
{
    void SetCursor(float direction, void* context);
    void IndicateLocation(unsigned short mask, void* context);
    void ChangeAmmo(unsigned short ammo, void* context);
    void ChangeHealth(unsigned short health, void* context);
    void UpdateInventory(void* context);
}
#endif /* defined(__Desktop_Adventure__HostCallbacks__) */
