//
//  CocoaSoundMixer.hpp
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#ifndef CocoaSoundMixer_cpp
#define CocoaSoundMixer_cpp

#include <stdio.h>
#import "SoundMixer.hpp"

class CococaSoundMixer : public SoundMixer {
public:
    void play(Sound* sound, Type type) override;
};
#endif /* CocoaSoundMixer_cpp */
