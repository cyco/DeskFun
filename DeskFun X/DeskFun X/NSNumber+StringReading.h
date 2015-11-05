//
//  NSNumber+StringReading.h
//  DeskFun X
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSNumber (StringReading)
+ (NSNumber*)parseHexString:(NSString*)string;
@end
