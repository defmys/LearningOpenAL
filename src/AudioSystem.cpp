#include "AudioSystem.h"
#include "helperfunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

void AudioSystem::Init()
{
    CreateDevice();
    CreateContext();
}

void AudioSystem::Shutdown()
{
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
