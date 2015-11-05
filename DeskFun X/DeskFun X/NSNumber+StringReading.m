//
//  NSNumber+StringReading.m
//  DeskFun X
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "NSNumber+StringReading.h"

@implementation NSNumber (StringReading)
+ (NSNumber*)parseHexString:(NSString*)string {
    return [NSNumber numberWithLong:strtol([string cStringUsingEncoding:NSUTF8StringEncoding], NULL, 0x10)];
}
@end
