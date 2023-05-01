#pragma once

#include "AudioSample.h"

#include <iostream>
#include <map>
#include <string>

struct ALCdevice;
struct ALCcontext;

class AudioSystem
{
public:
    explicit AudioSystem(const std::string& resourceRoot);

    void Init();
    void Shutdown();
    void Update();

    template<typename T = AudioSample, typename std::enable_if_t<std::is_base_of_v<AudioSample, T>, bool> = true>
    bool CreateAudioSample(const std::string& name, const char* resourcePath)
    {
        if (m_audioSamples.find(name) != m_audioSamples.end())
        {
            std::cerr << "Failed to create AudioSample. " << name << " already exists." << std::endl;
            return false;
        }

        std::unique_ptr<AudioSample> newSample = std::make_unique<T>();
        std::string fullPath(m_resourceRoot);
        fullPath.append(resourcePath);
        if (!newSample->LoadResource(fullPath.c_str()))
        {
            std::cerr << "Failed to create AudioSample. " << name << std::endl;
            return false;
        }

        m_audioSamples.emplace(name, std::move(newSample));

        return true;
    }

    AudioSample* GetSample(const std::string& name) const;
    void Play(const std::string& name);
    bool IsPlayingSometing() const;

private:
    void CreateDevice();
    void CreateContext();
    void InitListener();

private:
    const std::string m_resourceRoot;

    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;

    std::map<std::string, std::unique_ptr<AudioSample>> m_audioSamples;
};
