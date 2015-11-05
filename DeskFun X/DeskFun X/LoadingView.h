//
//  LoadingView.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface LoadingView : NSView
- (void)setLoadingImageWithData:(const unsigned char*)data size:(size_t)size andPalette:(const unsigned char*)pal;
@property (nonatomic) double progress;
@end
