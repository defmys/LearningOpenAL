#include <iostream>

#include "AL/alc.h"
#include "AL/al.h"

int main(int argc, const char** argv) 
{
    std::cout << "Hello" << std::endl;

    ALCdevice* device = alcOpenDevice(nullptr);
    assert(device);

    return 0;
}
