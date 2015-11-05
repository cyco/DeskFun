//
//  DAInteractiveTableView.h
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol DAInteravtiveTableViewDelegate <NSObject>

- (BOOL)tableView:(NSTableView*)tableView mouseDownWithEvent:(NSEvent*)event;
- (BOOL)tableView:(NSTableView*)tableView mouseUpWithEvent:(NSEvent*)event;

@end
@interface DAInteractiveTableView : NSTableView

@end
