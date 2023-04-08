#include "AudioSystem.h"
#include "AudioSample.h"
#include "helperfunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>
#include <iostream>

void AudioSystem::Init()
{
    CreateDevice();
    CreateContext();
}

void AudioSystem::Shutdown()
{
    if (!m_audioSamples.empty())
    {
        m_audioSamples.clear();
    }

    if (m_device)
    {
        ALCboolean bSuccess = ALC_FALSE;

        alcCall(alcMakeContextCurrent, m_device, bSuccess, nullptr);
        alcCall(alcDestroyContext, m_device, m_context);
        m_context = nullptr;

        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

void AudioSystem::Update()
{
    for (auto it = m_audioSamples.cbegin(); it != m_audioSamples.cend(); ++it)
    {
        it->second->Update();
    }
}

void AudioSystem::Play(const std::string &name)
{
    if (AudioSample* sample = GetSample(name))
    {
        sample->Play();
        sample->Update();
    }
}

bool AudioSystem::IsPlayingSometing() const
{
    for (auto it = m_audioSamples.cbegin(); it != m_audioSamples.cend(); ++it)
    {
        if (it->second->GetState() == AL_PLAYING)
        {
            return true;
        }
    }
    return false;
}

void AudioSystem::CreateDevice()
{
    if (!m_device)
    {
        m_device = alcOpenDevice(nullptr);
        assert(m_device);
    }
}

void AudioSystem::CreateContext()
{
    assert(alcCall(alcCreateContext, m_device, m_context, m_device, nullptr));
    assert(m_context);

    ALCboolean bSuccess = ALC_FALSE;
    assert(alcCall(alcMakeContextCurrent, m_device, bSuccess, m_context));
    assert(bSuccess == ALC_TRUE);
}

AudioSample* AudioSystem::GetSample(const std::string &name) const
{
    const auto it = m_audioSamples.find(name);
    if (it != m_audioSamples.cend())
    {
        return it->second.get();
    }

    return nullptr;
}
