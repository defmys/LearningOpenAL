#include <iostream>

#include "helperfunctions.h"

#include "AL/alc.h"
#include "AL/al.h"
#include "sndfile.h"

int main(int argc, const char** argv) 
{
    ALCdevice* device = alcOpenDevice(nullptr);
    assert(device);

    std::string fullPath(argv[0]);
    std::string exeDir = fullPath.substr(0, fullPath.rfind("/") + 1);

    std::string wavFile = exeDir + "resource/iamtheprotectorofthissystem.wav";
    SF_INFO sndInfo{};
    std::cout << wavFile << std::endl;
    SNDFILE* sndFile = sf_open(wavFile.c_str(), SFM_READ, &sndInfo);
    assert(sndFile);
    if (sndFile)
    {
        sf_close(sndFile);
    }

    return 0;
}
