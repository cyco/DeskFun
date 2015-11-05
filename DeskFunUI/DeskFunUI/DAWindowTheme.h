//
//  DAWindowTheme.h
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface DAWindowTheme : NSObject
+ (instancetype)sharedTheme;
@property (readonly) NSColor *backgroundColor;
@property (readonly) NSColor *bezelColorLight;
@property (readonly) NSColor *bezelColorDark;
@end
