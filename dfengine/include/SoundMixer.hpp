//
//  SoundMixer.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__SoundMixer__
#define __Desktop_Adventure__SoundMixer__

#include <stdio.h>
class Sound;
class SoundMixer
{
public:
    typedef enum : unsigned short {
        SoundMaster,
        SoundMusic,
        SoundEffect,
    } Type;

    void init(void);
    void cleanup(void);

    virtual void play(Sound* sound, Type type) = 0;
    void mute(Type type);
    void unmute(Type type);
    bool isMuted(Type type);

    void setVolume(unsigned short volume, Type type);
    unsigned short getVolume(Type type);

private:
    unsigned short masterVolume, effectVolume, musicVolume;
    bool masterMute, effectMute, musicMute;
};

#endif
