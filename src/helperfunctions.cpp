#include "helperfunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>

bool check_al_errors(const char* filename, uint32_t line)
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        std::cerr << "AL Error: (" << filename << ":" << line << ")" << std::endl;
        switch (error)
        {
        case AL_INVALID_ENUM:
            std::cerr << "AL_INVALID_ENUM";
            break;
        case AL_INVALID_NAME:
            std::cerr << "AL_INVALID_NAME";
            break;
        case AL_INVALID_OPERATION:
            std::cerr << "AL_INVALID_OPERATION";
            break;
        case AL_INVALID_VALUE:
            std::cerr << "AL_INVALID_VALUE";
            break;
        case AL_OUT_OF_MEMORY:
            std::cerr << "AL_OUT_OF_MEMORY";
            break;
        default:
            break;
        }
        std::cerr << std::endl;
        return false;
    }

    return true;
}

bool check_alc_errors(const char* filename, uint32_t line, ALCdevice* device)
{
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR)
    {
        std::cerr << "ALC Error: (" << filename << ":" << line << ")" << std::endl;
        switch (error)
        {
        case ALC_INVALID_CONTEXT: 
            std::cerr << "ALC_INVALID_CONTEXT";
            break;
        case ALC_INVALID_DEVICE:
            std::cerr << "ALC_INVALID_DEVICE";
            break;
        case ALC_INVALID_ENUM:
            std::cerr << "ALC_INVALID_ENUM";
            break;
        case ALC_INVALID_VALUE:
            std::cerr << "ALC_INVALID_VALUE";
            break;
        case ALC_OUT_OF_MEMORY:
            std::cerr << "ALC_OUT_OF_MEMORY";
            break;
        default:
            std::cerr << "Unknown ALC Error.";
        }
        std::cerr << std::endl;
        return false;
    }

    return true;
}
