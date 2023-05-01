#include <iostream>

#include "AudioSystem.h"
#include "AudioSample.h"
#include "AudioExample.h"
#include "helperfunctions.h"

#include "includes.inl"

#include "AL/alc.h"
#include "AL/al.h"
#include "sndfile.h"

#include <unistd.h>
#include <map>

int main(int argc, const char** argv) 
{
    if (argc < 2)
    {
        std::cerr << "usage: learning_openal {example}" << std::endl;
        return -1;
    }

    std::map<std::string, std::unique_ptr<AudioExample>> examples;
    #include "register_examples.inl";

    std::string fullPath(argv[0]);
    std::string exeDir = fullPath.substr(0, fullPath.rfind("/") + 1);
    exeDir.append("resource/");

    std::string exampleName(argv[1]);
    const auto exampleIt = examples.find(exampleName);
    if (exampleIt == examples.cend())
    {
        std::cerr << "No example named " << exampleName << std::endl;
        return -1;
    }

    std::unique_ptr<AudioSystem> audioSystem = std::make_unique<AudioSystem>(exeDir);
    audioSystem->Init();

    AudioExample* pExample = exampleIt->second.get();
    if (pExample)
    {
        pExample->Prepare(audioSystem.get());
        pExample->Run();
    }

    /*
    std::string wavFile = exeDir + "resource/engine-mono.mp3";
    audioSystem->CreateAudioSample("test", wavFile.c_str());
    audioSystem->GetSample("test")->SetPos(-2.f, 0, 1);
    audioSystem->GetSample("test")->SetVelocity(90.f, 0, 0);
    audioSystem->Play("test");
    */
    
    while (audioSystem->IsPlayingSometing())
    {
        audioSystem->Update();
        usleep(30000.f);
    };

    audioSystem->Shutdown();

    return 0;
}
