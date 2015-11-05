//
//  HostCallbacks.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "HostCallbacks.h"

#import "DACursor.h"
#import "DAGameWindowController.h"
#import <DeskFunUI/DALocationView.h>

void Host::SetCursor(float direction, void* context)
{
    [DACursor pop];
    DAGameWindowController *controller = (__bridge DAGameWindowController*)context;
    NSPoint p = [NSEvent mouseLocation];
    if(NSPointInRect([[controller window] convertScreenToBase:p], NSInsetRect([[controller gameView] frame], -10, -10)))
        [[DACursor cursorWithDirection:direction] push];
}

void Host::IndicateLocation(unsigned short mask, void* context)
{
    DAGameWindowController *controller = (__bridge DAGameWindowController*)context;
    DALocationView *locationView = [controller locationView];

    [locationView setLeft:(mask & DirectionWest) != 0];
    [locationView setRight:(mask & DirectionEast) != 0];
    [locationView setUp:(mask & DirectionNorth) != 0];
    [locationView setDown:(mask & DirectionSouth) != 0];
}

void Host::ChangeAmmo(unsigned short ammo, void* context)
{}
void Host::ChangeHealth(unsigned short health, void* context)
{}

void Host::UpdateInventory(void* context)
{
    DAGameWindowController *controller = (__bridge DAGameWindowController*)context;
    [[controller inventoryTable] reloadData];
}