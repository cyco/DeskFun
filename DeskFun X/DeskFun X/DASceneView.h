//
//  DAMapView.h
//  DA
//
//  Created by Christoph Leimbrock on 10/15/13.
//  Copyright (c) 2013 CCL. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "Engine.hpp"

@interface DASceneView : NSOpenGLView
@property (nonatomic) Engine* engine;
@end
