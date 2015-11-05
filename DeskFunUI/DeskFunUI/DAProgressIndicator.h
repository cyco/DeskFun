//
//  DAProgressIndicator.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "DABezelView.h"

@interface DAProgressIndicator : DABezelView
@property double minValue, doubleValue, maxValue;
@property BOOL indeterminate;
@property BOOL segmented, adjustEverySegment;
@property (strong) NSColor *indicatorColor;
@end
