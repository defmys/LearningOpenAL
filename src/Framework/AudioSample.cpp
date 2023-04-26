#include "AudioSample.h"
#include "helperfunctions.h"

#include <AL/alext.h>

#include <cassert>
#include <iostream>

enum class FormatType {
    Int16,
    Float,
    IMA4,
    MSADPCM
};

AudioSample::~AudioSample()
{
    if (m_source)
    {
        alCall(alDeleteSources, 1, &m_source);
        m_source = AL_INVALID;

        std::cout << __FUNCTION__ << std::endl;
    }
    if (m_buffer != AL_INVALID)
    {
        alCall(alDeleteBuffers, 1, &m_buffer);
        m_buffer = AL_INVALID;
    }
}

bool AudioSample::LoadResource(const char *filename)
{
    if (m_buffer == AL_INVALID)
    {
        SF_INFO sndInfo{};
        SNDFILE* sndFile = sf_open(filename, SFM_READ, &sndInfo);
        if (!sndFile)
        {
            std::cerr << "failed to load " << filename << std::endl;
            return false;
        }

        if (!LoadFormat(sndFile, sndInfo))
        {
            return false;
        }

        m_context.sampleRate = sndInfo.samplerate;
        CreateBuffer();

        if (!m_bPersistantMemBuf && !m_context.membuf.empty())
        {
            m_context.membuf.clear();
        }
        sf_close(sndFile);

        CreateSource();
    }

    return true;
}

bool AudioSample::LoadFormat(SNDFILE* sndFile, const SF_INFO& sndInfo)
{
    FormatType sampleFormat = FormatType::Int16;
    switch ((sndInfo.format & SF_FORMAT_SUBMASK))
    {
        case SF_FORMAT_PCM_24:
        case SF_FORMAT_PCM_32:
        case SF_FORMAT_FLOAT:
        case SF_FORMAT_DOUBLE:
        case SF_FORMAT_VORBIS:
        case SF_FORMAT_OPUS:
        case SF_FORMAT_ALAC_20:
        case SF_FORMAT_ALAC_24:
        case SF_FORMAT_ALAC_32:
        case 0x0080/*SF_FORMAT_MPEG_LAYER_I*/:
        case 0x0081/*SF_FORMAT_MPEG_LAYER_II*/:
        case 0x0082/*SF_FORMAT_MPEG_LAYER_III*/:
            if(alIsExtensionPresent("AL_EXT_FLOAT32"))
                sampleFormat = FormatType::Float;
            break;
        case SF_FORMAT_IMA_ADPCM:
            /* ADPCM formats require setting a block alignment as specified in the
            * file, which needs to be read from the wave 'fmt ' chunk manually
            * since libsndfile doesn't provide it in a format-agnostic way.
            */
            if(sndInfo.channels <= 2 && (sndInfo.format&SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                && alIsExtensionPresent("AL_EXT_IMA4")
                && alIsExtensionPresent("AL_SOFT_block_alignment"))
                sampleFormat = FormatType::IMA4;
            break;
        case SF_FORMAT_MS_ADPCM:
            if(sndInfo.channels <= 2 && (sndInfo.format&SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                && alIsExtensionPresent("AL_SOFT_MSADPCM")
                && alIsExtensionPresent("AL_SOFT_block_alignment"))
                sampleFormat = FormatType::MSADPCM;
            break;
    }

    ALint byteblockalign = 0;
    if(sampleFormat == FormatType::IMA4 || sampleFormat == FormatType::MSADPCM)
    {
        /* For ADPCM, lookup the wave file's "fmt " chunk, which is a
         * WAVEFORMATEX-based structure for the audio format.
         */
        SF_CHUNK_INFO inf = { 
            "fmt",  // id
            4,      // id_size
            0,      // datalen
            nullptr // data 
        };
        SF_CHUNK_ITERATOR *iter = sf_get_chunk_iterator(sndFile, &inf);

        /* If there's an issue getting the chunk or block alignment, load as
         * 16-bit and have libsndfile do the conversion.
         */
        if(!iter || sf_get_chunk_size(iter, &inf) != SF_ERR_NO_ERROR || inf.datalen < 14)
        {
            sampleFormat = FormatType::Int16;
        }
        else
        {
            ALubyte* fmtBuf = static_cast<ALubyte*>(malloc(inf.datalen));
            inf.data = fmtBuf;

            if(sf_get_chunk_data(iter, &inf) != SF_ERR_NO_ERROR)
            {
                sampleFormat = FormatType::Int16;
            }
            else
            {
                /* Read the nBlockAlign field, and convert from bytes- to
                 * samples-per-block (verifying it's valid by converting back
                 * and comparing to the original value).
                 */
                byteblockalign = fmtBuf[12] | (fmtBuf[13]<<8);

                if(sampleFormat == FormatType::IMA4)
                {
                    m_context.splblockalign = (byteblockalign/sndInfo.channels - 4)/4*8 + 1;
                    if(m_context.splblockalign < 1 || ((m_context.splblockalign-1)/2 + 4)*sndInfo.channels != byteblockalign)
                    {
                        sampleFormat = FormatType::Int16;
                    }
                }
                else
                {
                    m_context.splblockalign = (byteblockalign/sndInfo.channels - 7)*2 + 2;
                    if(m_context.splblockalign < 2 || ((m_context.splblockalign-2)/2 + 7)*sndInfo.channels != byteblockalign)
                    {
                        sampleFormat = FormatType::Int16;
                    }
                }
            }

            free(fmtBuf);
        }
    }

    if(sampleFormat == FormatType::Int16)
    {
        m_context.splblockalign = 1;
        byteblockalign = sndInfo.channels * 2;
    }
    else if(sampleFormat == FormatType::Float)
    {
        m_context.splblockalign = 1;
        byteblockalign = sndInfo.channels * 4;
    }

    /* Figure out the OpenAL format from the file and desired sample type. */
    if(sndInfo.channels == 1)
    {
        if(sampleFormat == FormatType::Int16)
            m_context.format = AL_FORMAT_MONO16;
        else if(sampleFormat == FormatType::Float)
            m_context.format = AL_FORMAT_MONO_FLOAT32;
        else if(sampleFormat == FormatType::IMA4)
            m_context.format = AL_FORMAT_MONO_IMA4;
        else if(sampleFormat == FormatType::MSADPCM)
            m_context.format = AL_FORMAT_MONO_MSADPCM_SOFT;
    }
    else if(sndInfo.channels == 2)
    {
        if(sampleFormat == FormatType::Int16)
            m_context.format = AL_FORMAT_STEREO16;
        else if(sampleFormat == FormatType::Float)
            m_context.format = AL_FORMAT_STEREO_FLOAT32;
        else if(sampleFormat == FormatType::IMA4)
            m_context.format = AL_FORMAT_STEREO_IMA4;
        else if(sampleFormat == FormatType::MSADPCM)
            m_context.format = AL_FORMAT_STEREO_MSADPCM_SOFT;
    }
    else if(sndInfo.channels == 3)
    {
        if(sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sampleFormat == FormatType::Int16)
                m_context.format = AL_FORMAT_BFORMAT2D_16;
            else if(sampleFormat == FormatType::Float)
                m_context.format = AL_FORMAT_BFORMAT2D_FLOAT32;
        }
    }
    else if(sndInfo.channels == 4)
    {
        if(sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sampleFormat == FormatType::Int16)
                m_context.format = AL_FORMAT_BFORMAT3D_16;
            else if(sampleFormat == FormatType::Float)
                m_context.format = AL_FORMAT_BFORMAT3D_FLOAT32;
        }
    }
    if(!m_context.format)
    {
        std::cerr << "Unsupported channel count: " << sndInfo.channels << std::endl;
        return false;
    }
    if(sndInfo.frames/m_context.splblockalign > (sf_count_t)(INT_MAX/byteblockalign))
    {
        std::cerr << "Too many samples (" << sndInfo.frames << ")" << std::endl;
        return false;
    }

    /* Decode the whole audio file to a buffer. */
    m_context.membuf.resize((size_t)(sndInfo.frames / m_context.splblockalign * byteblockalign));

    sf_count_t numFrames = 0;
    if(sampleFormat == FormatType::Int16)
    {
        numFrames = sf_readf_short(sndFile, (short*)m_context.membuf.data(), sndInfo.frames);
    }
    else if(sampleFormat == FormatType::Float)
    {
        numFrames = sf_readf_float(sndFile, (float*)m_context.membuf.data(), sndInfo.frames);
    }
    else
    {
        sf_count_t count = sndInfo.frames / m_context.splblockalign * byteblockalign;
        numFrames = sf_read_raw(sndFile, m_context.membuf.data(), count);
        if(numFrames > 0)
        {
            numFrames = numFrames / byteblockalign * m_context.splblockalign;
        }
    }

    if(numFrames < 1)
    {
        m_context.membuf.clear();
        std::cerr << "Failed to read samples (" << numFrames << ")" << std::endl;
        return false;
    }

    m_context.numBytes = (ALsizei)(numFrames / m_context.splblockalign * byteblockalign);

    return true;
}

void AudioSample::UpdatePos()
{
    alCall(alSourcefv, m_source, AL_POSITION, m_pos.data());
}

void AudioSample::UpdateVelocity()
{
    alCall(alSourcefv, m_source, AL_VELOCITY, m_velocity.data());
}

bool AudioSample::CreateBuffer()
{
    if (!alCall(alGenBuffers, 1, &m_buffer))
    {
        std::cerr << "failed to create al buffer" << std::endl;
        return false;
    }

    if(m_context.splblockalign > 1)
    {
        alCall(alBufferi, m_buffer, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, m_context.splblockalign);
    }
    alCall(alBufferData, m_buffer, m_context.format, m_context.membuf.data(), m_context.numBytes, m_context.sampleRate);

    return true;
}

void AudioSample::CreateSource()
{
    alCall(alGenSources, 1, &m_source);
    alCall(alSourcef, m_source, AL_PITCH, 1);
    alCall(alSourcef, m_source, AL_GAIN, 1.0f);
    alCall(alSourcei, m_source, AL_LOOPING, AL_FALSE);
    alCall(alSourcei, m_source, AL_BUFFER, m_buffer);
    UpdatePos();
    UpdateVelocity();
}

void AudioSample::Play()
{
    if (m_state == AL_INITIAL && m_source != AL_INVALID)
    {
        alCall(alSourcePlay, m_source);
        m_state = AL_PLAYING;
    }
}

void AudioSample::Update()
{
    if (m_source)
    {
        alCall(alGetSourcei, m_source, AL_SOURCE_STATE, &m_state);
        m_pos[0] += 0.016;
        UpdatePos();
    }
}

void AudioSample::SetPos(float x, float y, float z)
{
    m_pos = {x, y, z};
    UpdatePos();
}

void AudioSample::SetVelocity(float x, float y, float z)
{
    m_velocity = {x, y, z};
    UpdateVelocity();
}

AudioStreamingSample::AudioStreamingSample()
{
    m_bPersistantMemBuf = true;
}

AudioStreamingSample::~AudioStreamingSample()
{
    ALint numQueued = 0;
    alCall(alGetSourcei, m_source, AL_BUFFERS_QUEUED, &numQueued);

    if (numQueued > 0)
    {
        std::vector<ALuint> buffers(numQueued);
        alCall(alSourceUnqueueBuffers, m_source, numQueued, buffers.data());
    }

    alCall(alDeleteBuffers, m_buffers.size(), m_buffers.data());
}

void AudioStreamingSample::Update()
{
    if (m_source)
    {
        alCall(alGetSourcei, m_source, AL_SOURCE_STATE, &m_state);

        if (GetState() == AL_PLAYING)
        {
            ALint processed = 0;
            if (alCall(alGetSourcei, m_source, AL_BUFFERS_PROCESSED, &processed))
            {
                while (processed > 0)
                {
                    ALuint buffer;
                    alCall(alSourceUnqueueBuffers, m_source, 1, &buffer);

                    std::vector<uint8_t> data;
                    data.resize(BUFFER_SIZE);
                    memset(data.data(), 0, BUFFER_SIZE);

                    size_t dataSizeToCopy = BUFFER_SIZE;
                    if (m_cursor + BUFFER_SIZE > m_context.membuf.size())
                    {
                        dataSizeToCopy = m_context.membuf.size() - m_cursor;
                    }
                    memcpy(data.data(), &m_context.membuf[m_cursor], dataSizeToCopy);
                    m_cursor += dataSizeToCopy;

                    /*
                    // loop
                    if (dataSizeToCopy < BUFFER_SIZE)
                    {
                        m_cursor = 0;
                        memcpy(&data[dataSizeToCopy], &m_context.membuf[m_cursor], BUFFER_SIZE - dataSizeToCopy);
                        m_cursor = BUFFER_SIZE - dataSizeToCopy;
                    }
                    */

                    if (dataSizeToCopy > 0) 
                    {
                        alCall(alBufferData, buffer, m_context.format, data.data(), dataSizeToCopy, m_context.sampleRate);
                        alCall(alSourceQueueBuffers, m_source, 1, &buffer);
                    }

                    processed--;
                }
            }
        }
    }
    else
    {
        m_source = AL_STOPPED;
    }
}

bool AudioStreamingSample::CreateBuffer()
{
    if (!alCall(alGenBuffers, NUM_BUFFERS, m_buffers.data()))
    {
        std::cerr << "failed to create al buffer" << std::endl;
        return false;
    }

    for (size_t i = 0; i < NUM_BUFFERS; ++i)
    {
        alCall(alBufferData, m_buffers[i], m_context.format, &m_context.membuf[i * BUFFER_SIZE], BUFFER_SIZE, m_context.sampleRate);
    }
    

    return true;
}

void AudioStreamingSample::CreateSource()
{
    alCall(alGenSources, 1, &m_source);
    alCall(alSourcef, m_source, AL_PITCH, 1);
    alCall(alSourcef, m_source, AL_GAIN, 1.0f);
    alCall(alSourcei, m_source, AL_LOOPING, AL_FALSE);
    UpdatePos();
    UpdateVelocity();

    alCall(alSourceQueueBuffers, m_source, NUM_BUFFERS, m_buffers.data());
}
