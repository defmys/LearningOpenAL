#pragma once

#include "Examples/AudioExample.h"
#include <array>

class PosAndVelocity: public AudioExample
{
public:
    virtual void Run() override;
    virtual void Update() override;

private:
    class AudioSample* m_sample = nullptr;
    std::array<float, 3> m_pos = {0.f};
};
