//
//  SetupWindowController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "SetupWindowController.h"

#import "AppDelegate.h"
#import "GameDataParser.hpp"
#include "Palette.h"

@interface SetupWindowController ()
@end

@implementation SetupWindowController

- (void)windowDidLoad
{
    [super windowDidLoad];

    AppDelegate *dele = (AppDelegate*)[NSApp delegate];
    GameDataParser* data = [dele GameDataParser];

    const unsigned char * palette = indys_pallette;
    if(data->_isYodaVersion){
        palette = yodas_palette;
    }

    [[self loadingView] setLoadingImageWithData:data->_loadingImage size:data->_loadingImagePixelCount andPalette:palette];
}

- (NSString*)windowNibName
{
    return @"SetupWindowController";
}

@end
