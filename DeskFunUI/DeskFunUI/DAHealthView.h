//
//  DAHealthView.h
//  DAGame
//
//  Created by Christoph Leimbrock on 25/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DABezelView.h"
@interface DAHealthView : DABezelView

@property (nonatomic) CGFloat health;

@property NSColor *goodColor;
@property NSColor *mediumColor;
@property NSColor *badColor;
@property NSColor *criticalColor;
@end
