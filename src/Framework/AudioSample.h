#pragma once

#include <AL/al.h>
#include "sndfile.h"

#include <array>
#include <vector>


struct AudioFormatContext
{
    ALenum format;
    std::vector<uint8_t> membuf;
    ALsizei numBytes;
    ALint splblockalign;
    int sampleRate;
};

class AudioSample
{
public:
    virtual ~AudioSample();

    bool LoadResource(const char* filename);
    void Play();
    virtual void Update();
    ALint GetState() const { return m_state; }

    void SetPos(float x, float y, float z);
    void SetVelocity(float x, float y, float z);

protected:
    virtual bool CreateBuffer();
    virtual void CreateSource();

    void UpdatePos();
    void UpdateVelocity();

private:
    bool LoadFormat(SNDFILE* sndFile, const SF_INFO& sndInfo);

private:
    ALuint m_buffer = AL_INVALID;

    std::array<float, 3> m_pos = {0.f, 0.f, 0.f};  // x, y, z
    std::array<float, 3> m_velocity = {0.f, 0.f, 0.f};  // x, y, z

protected:
    AudioFormatContext m_context;
    bool m_bPersistantMemBuf = false;
    ALuint m_source = AL_INVALID;
    ALint m_state = AL_INITIAL;
};

class AudioStreamingSample : public AudioSample
{
public:
    AudioStreamingSample();
    virtual ~AudioStreamingSample();
    virtual void Update() override;

protected:
    virtual bool CreateBuffer() override;
    virtual void CreateSource() override;

private:
    static constexpr size_t NUM_BUFFERS = 4;
    static constexpr size_t BUFFER_SIZE = 128 *1024;
    std::array<ALuint, NUM_BUFFERS> m_buffers;

    size_t m_cursor = NUM_BUFFERS * BUFFER_SIZE;
};
