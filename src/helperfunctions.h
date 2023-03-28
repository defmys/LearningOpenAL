#pragma once
#include "AL/al.h"
#include "AL/alc.h"
#include <type_traits>

bool check_al_errors(const char* filename, uint32_t line, ALCdevice* device)
{
    ALenum error = alcGetError(device);
    if (error != AL_NO_ERROR)
    {
        std::cerr << "AL Error: (" << filename << ":" << line << ")" << std::endl;
        switch (error)
        {
        case AL_INVALID_NAME: 
            std::cerr << "AL_INVALID_NAME";
            break;
        case AL_INVALID_ENUM:
            std::cerr << "AL_INVALID_ENUM";
            break;
        case AL_INVALID_VALUE:
            std::cerr << "AL_INVALID_VALUE";
            break;
        case AL_INVALID_OPERATION:
            std::cerr << "AL_INVALID_OPERATION";
            break;
        case AL_OUT_OF_MEMORY:
            std::cerr << "AL_OUT_OF_MEMORY";
            break;
        default:
            std::cerr << "Unknown AL Error.";
        }
        std::cerr << std::endl;
        return false;
    }

    return true;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
{
    auto ret = function(std::forward(params...));
    check_al_errors(filename, line, device);
    return ret;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward(params...));
    return check_al_errors(filename, line, device);
}

#define alCall(function, device, ...) alCallImpl(__FILE__, __LINE__, function, device, __VA__ARGS__)
