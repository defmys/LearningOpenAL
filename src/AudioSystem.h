#pragma once

#include <map>
#include <string>

struct ALCdevice;
struct ALCcontext;
class AudioSample;

class AudioSystem
{
public:
    void Init();
    void Shutdown();
    void Update();

    bool CreateAudioSample(const std::string& name, const char* resourcePath);
    void Play(const std::string& name);
    bool IsPlayingSometing() const;

private:
    void CreateDevice();
    void CreateContext();
    AudioSample* GetSample(const std::string& name) const;

private:
    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;

    std::map<std::string, std::unique_ptr<AudioSample>> m_audioSamples;
};
