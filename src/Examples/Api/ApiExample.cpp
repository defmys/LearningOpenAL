#include "ApiExample.h"

#include "Framework/AudioSystem.h"
#include <string>

void ApiExample::Prepare(AudioSystem* pAudioSystem)
{
    AudioExample::Prepare(pAudioSystem);
}

void ApiExample::Run()
{
    const std::string sampleName("test");
    std::string wavFile = "Api/iamtheprotectorofthissystem.wav";
    // std::string wavFile = exeDir + "resource/iamtheprotectorofthissystem-mono.wav";
    m_pAudioSystem->CreateAudioSample(sampleName, wavFile.c_str());
    m_pAudioSystem->GetSample(sampleName)->SetPos(-2.f, 0, 1);
    m_pAudioSystem->GetSample(sampleName)->SetVelocity(90.f, 0, 0);
    m_pAudioSystem->Play(sampleName);
}
