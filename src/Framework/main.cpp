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
    
    while (audioSystem->IsPlayingSometing())
    {
        audioSystem->Update();

        if (pExample)
        {
            pExample->Update();
        }

        usleep(30000.f);
    };

    audioSystem->Shutdown();

    return 0;
}
