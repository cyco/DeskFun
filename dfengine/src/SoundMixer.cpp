//
//  SoundMixer.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "SoundMixer.hpp"
#include "Sound.hpp"

void SoundMixer::init(void)
{}

void SoundMixer::cleanup(void)
{}

void SoundMixer::mute(Type type)
{
    switch (type) {
        case SoundEffect: effectMute = true; break;
        case SoundMusic:  musicMute  = true; break;
        case SoundMaster: masterMute = true; break;
    }
}

bool SoundMixer::isMuted(Type type)
{

    switch (type) {
        case SoundEffect: return effectMute;
        case SoundMusic:  return musicMute;
        case SoundMaster: return masterMute;
    }
}

void SoundMixer::unmute(Type type)
{
    switch (type) {
        case SoundEffect: effectMute = false; break;
        case SoundMusic:  musicMute  = false; break;
        case SoundMaster: masterMute = false; break;
    }
}

void SoundMixer::setVolume(unsigned short volume, Type type)
{
    switch (type) {
        case SoundEffect: effectVolume = volume; break;
        case SoundMusic:  musicVolume  = volume; break;
        case SoundMaster: masterVolume = volume; break;
    }
}

unsigned short SoundMixer::getVolume(Type type)
{
    switch (type) {
        case SoundEffect: return effectVolume; break;
        case SoundMusic: return musicVolume; break;
        case SoundMaster: return masterVolume; break;
    }
}