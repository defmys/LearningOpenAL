#include <iostream>

#include "AudioSystem.h"
#include "AudioSample.h"
#include "helperfunctions.h"

#include "AL/alc.h"
#include "AL/al.h"
#include "sndfile.h"

#include <unistd.h>

int main(int argc, const char** argv) 
{
    std::string fullPath(argv[0]);
    std::string exeDir = fullPath.substr(0, fullPath.rfind("/") + 1);

    AudioSystem audioSystem;
    audioSystem.Init();

    // std::string wavFile = exeDir + "resource/iamtheprotectorofthissystem.wav";
    // std::string wavFile = exeDir + "resource/iamtheprotectorofthissystem-mono.wav";
    std::string wavFile = exeDir + "resource/engine-mono.mp3";
    audioSystem.CreateAudioSample("test", wavFile.c_str());
    audioSystem.GetSample("test")->SetPos(-2.f, 0, 1);
    audioSystem.GetSample("test")->SetVelocity(90.f, 0, 0);
    audioSystem.Play("test");

    //std::string swooshFile = exeDir + "resource/swoosh.mp3";
    //audioSystem.CreateAudioSample<AudioStreamingSample>("swoosh", swooshFile.c_str());
    // audioSystem.Play("swoosh");
    
    while (audioSystem.IsPlayingSometing())
    {
        audioSystem.Update();
        usleep(30000.f);
    };

    audioSystem.Shutdown();

    return 0;
}