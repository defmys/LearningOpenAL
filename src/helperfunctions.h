#pragma once

#include <cstdint>
#include <type_traits>

struct ALCdevice;

bool check_al_errors(const char* filename, uint32_t line, ALCdevice* device);

template<typename alFunction, typename ReturnType, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, ReturnType& ret, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
{
    ret = function(std::forward<Params>(params)...);
    return check_al_errors(filename, line, device);
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename, uint32_t line, alFunction function, ALCdevice* device, Params... params)
    -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return check_al_errors(filename, line, device);
}

#define alCall(function, device, ...) alCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)
