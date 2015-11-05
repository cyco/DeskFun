//
//  DAInteractiveTableView.m
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAInteractiveTableView.h"

@implementation DAInteractiveTableView

- (void)mouseDown:(NSEvent *)theEvent
{
    if(![[self delegate] respondsToSelector:@selector(tableView:mouseDownWithEvent:)] || ![(id <DAInteravtiveTableViewDelegate>)[self delegate] tableView:self mouseDownWithEvent:theEvent])
    {
        [super mouseDown:theEvent];
    }
}


- (void)mouseUp:(NSEvent *)theEvent
{
    if(![[self delegate] respondsToSelector:@selector(tableView:mouseUpWithEvent:)] || ![(id <DAInteravtiveTableViewDelegate>)[self delegate] tableView:self mouseUpWithEvent:theEvent])
    {
        [super mouseUp:theEvent];
    }
}



@end
