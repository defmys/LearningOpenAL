#include "Streaming.h"
#include "Framework/AudioSample.h"
#include "Framework/AudioSystem.h"
#include <string>

void Streaming::Run()
{
    std::string swooshFile = "Streaming/swoosh.mp3";
    m_pAudioSystem->CreateAudioSample<AudioStreamingSample>("swoosh", swooshFile.c_str());
    m_pAudioSystem->Play("swoosh");
}