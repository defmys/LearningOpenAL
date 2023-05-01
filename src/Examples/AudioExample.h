#pragma once
#include <cassert>

class AudioSystem;

class AudioExample
{
public:
    virtual ~AudioExample() = default;

    virtual void Prepare(AudioSystem* pAudioSystem)
    {
        assert(pAudioSystem);
        m_pAudioSystem = pAudioSystem;
    };
    virtual void Run() = 0;
    virtual void Update() {}; 

protected:
    AudioSystem* m_pAudioSystem = nullptr;
};
