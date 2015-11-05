//
//  DAOldScroller.h
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DABezelView.h"

@interface DAScroller : DABezelView
- (void)reflectScrolledClipView:(NSClipView *)aClipView;
@end
