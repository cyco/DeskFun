//
//  CocoaSoundMixer.cpp
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#include "CocoaSoundMixer.hpp"
#include "Sound.hpp"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

void CococaSoundMixer::play(Sound *sound, Type type)
{
    if(isMuted(type) || isMuted(Type::SoundMaster)) return;

    NSBundle *mainBundle = [NSBundle mainBundle];
    NSURL *url = [mainBundle URLForResource:[[NSString stringWithCString:sound->getFileName().c_str() encoding:NSUTF8StringEncoding] stringByDeletingPathExtension] withExtension:@"wav" subdirectory:@"sfx"];
   NSSound *s = [[NSSound alloc] initWithContentsOfURL:url byReference:NO];
    [s play];
}