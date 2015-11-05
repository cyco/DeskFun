//
//  DAWindowTheme.m
//  DAGame
//
//  Created by Christoph Leimbrock on 27/10/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import "DAWindowTheme.h"
@interface DAWindowTheme ()
@end
@implementation DAWindowTheme
+ (instancetype)sharedTheme
{
    static DAWindowTheme *sharedTheme;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedTheme = [[DAWindowTheme alloc] init];

        sharedTheme->_backgroundColor = [NSColor colorWithDeviceRed:212/255.0 green:208/255.0 blue:200/255.0 alpha:1.0];
        sharedTheme->_bezelColorDark  = [NSColor colorWithDeviceWhite:0.5 alpha:1.0];
        sharedTheme->_bezelColorLight = [NSColor colorWithDeviceWhite:1.0 alpha:1.0];
    });
    return sharedTheme;
}

@end
