#include "AudioExample.h"
#include <cassert>

void AudioExample::Prepare(AudioSystem* pAudioSystem)
{
    assert(pAudioSystem);
    m_pAudioSystem = pAudioSystem;
}
