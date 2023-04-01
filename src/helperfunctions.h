#pragma once

#include <cstdint>
#include <type_traits>

struct ALCdevice;

bool check_al_errors(const char* filename, uint32_t line);

template<typename alFunction, typename ReturnType, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, ReturnType& ret, Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
    ret = function(std::forward<Params>(params)...);
    return check_al_errors(filename, line);
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return check_al_errors(filename, line);
}

bool check_alc_errors(const char* filename, uint32_t line, ALCdevice* device);

template<typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char* filename, uint32_t line, alcFunction function, ReturnType& ret, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
    ret = function(std::forward<Params>(params)...);
    return check_alc_errors(filename, line, device);
}

template<typename alcFunction, typename... Params>
auto alcCallImpl(const char* filename, uint32_t line, alcFunction function, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return check_alc_errors(filename, line, device);
}

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)
