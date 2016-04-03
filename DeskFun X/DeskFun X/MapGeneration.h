//
//  MapGeneration.h
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GameDataParser.hpp"
#import "MapView.h"
@interface MapGeneration : NSWindowController
@property (nonatomic, assign) GameDataParser *data;
@property (nonatomic, assign) IBOutlet MapView *mapView;
@property (nonatomic, assign) IBOutlet NSSlider *sizeSlider;
@property (nonatomic, assign) IBOutlet NSTextField *seedField;

- (IBAction)previousWorld:(id)sender;
- (IBAction)nextWorld:(id)sender;
- (IBAction)randomWorld:(id)sender;

- (IBAction)generateWorld:(id)sender;
@end
