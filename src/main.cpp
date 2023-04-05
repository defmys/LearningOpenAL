#include <iostream>

#include "AudioSystem.h"
#include "AudioSample.h"
#include "helperfunctions.h"

#include "AL/alc.h"
#include "AL/al.h"
#include "sndfile.h"

int main(int argc, const char** argv) 
{
    std::string fullPath(argv[0]);
    std::string exeDir = fullPath.substr(0, fullPath.rfind("/") + 1);

    AudioSystem audioSystem;
    audioSystem.Init();

    std::string wavFile = exeDir + "resource/iamtheprotectorofthissystem.wav";
    AudioSample* sample = new AudioSample;
    sample->LoadResource(wavFile.c_str());
    sample->Play();

    while (sample->GetState() == AL_PLAYING)
    {
        sample->Update();
    }
    delete(sample);
    sample = nullptr;

    audioSystem.Shutdown();

    return 0;
}
