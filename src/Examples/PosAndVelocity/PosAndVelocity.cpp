#include "PosAndVelocity.h"
#include "Framework/AudioSystem.h"
#include <string>

void PosAndVelocity::Run()
{
    const char keyName[] = "pos_example";
    std::string wavFile = "PosAndVelocity/engine-mono.mp3";
    if (m_pAudioSystem->CreateAudioSample(keyName, wavFile.c_str()))
    {
        m_sample = m_pAudioSystem->GetSample(keyName);
        if (m_sample)
        {
            m_pos = {-5.f, 0.f, 1.f};
            m_sample->SetPos(m_pos[0], m_pos[1], m_pos[2]);
            m_sample->SetVelocity(100.f, 0, 0);
            m_pAudioSystem->Play(keyName);
        }
    }
}

void PosAndVelocity::Update()
{
    m_pos[0] += 0.033;
    m_sample->SetPos(m_pos[0], m_pos[1], m_pos[2]);
}