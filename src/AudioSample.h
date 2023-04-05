#pragma once

#include <AL/al.h>
#include "sndfile.h"


struct AudioFormatContext
{
    ALenum format;
    void* membuf;
    ALsizei numBytes;
    ALint splblockalign;
};

class AudioSample
{
public:
    ~AudioSample();

    bool LoadResource(const char* filename);
    void Play();
    void Update();
    ALint GetState() const { return m_state; }

private:
    bool LoadFormat(SNDFILE* sndFile, const SF_INFO& sndInfo, AudioFormatContext& formatContext);
    void CreateSource();

private:
    ALuint m_buffer = AL_INVALID;
    ALuint m_source = AL_INVALID;
    ALint m_state = AL_INITIAL;
};
