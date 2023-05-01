#pragma once

#include "Framework/AudioExample.h"

class AudioSystem;

class ApiExample: public AudioExample
{
public:
    virtual void Prepare(AudioSystem* pAudioSystem) override;
    virtual void Run() override;
};
