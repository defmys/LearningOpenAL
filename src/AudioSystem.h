#pragma once

struct ALCdevice;
struct ALCcontext;

class AudioSystem
{
public:
    void Init();
    void Shutdown();

private:
    void CreateDevice();
    void CreateContext();

private:
    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;
};
