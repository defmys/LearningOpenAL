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
    }
    if (m_buffer)
    {
        alCall(alDeleteBuffers, 1, &m_buffer);
        m_buffer = AL_INVALID;
    }
}

void AudioSample::LoadResource(const char *filename)
{
    if (m_buffer == AL_INVALID)
    {
        SF_INFO sndInfo{};
        SNDFILE* sndFile = sf_open(filename, SFM_READ, &sndInfo);
        if (!sndFile)
        {
            std::cerr << "failed to load " << filename << std::endl;
            return;
        }

        AudioFormatContext formatContext{};
        if (LoadFormat(sndFile, sndInfo, formatContext))
        {
            if (alCall(alGenBuffers, 1, &m_buffer))
            {
                if(formatContext.splblockalign > 1)
                {
                    alCall(alBufferi, m_buffer, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, formatContext.splblockalign);
                }
                alCall(alBufferData, m_buffer, formatContext.format, formatContext.membuf, formatContext.numBytes, sndInfo.samplerate);

                if (formatContext.membuf)
                {
                    free(formatContext.membuf);
                    formatContext.membuf = nullptr;
                }

                CreateSource();
            }
            else
            {
                std::cerr << "failed to create al buffer for " << filename << std::endl;
            }
        }

        sf_close(sndFile);
    }
}

bool AudioSample::LoadFormat(SNDFILE* sndFile, const SF_INFO& sndInfo, AudioFormatContext& formatContext)
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
                    formatContext.splblockalign = (byteblockalign/sndInfo.channels - 4)/4*8 + 1;
                    if(formatContext.splblockalign < 1 || ((formatContext.splblockalign-1)/2 + 4)*sndInfo.channels != byteblockalign)
                    {
                        sampleFormat = FormatType::Int16;
                    }
                }
                else
                {
                    formatContext.splblockalign = (byteblockalign/sndInfo.channels - 7)*2 + 2;
                    if(formatContext.splblockalign < 2 || ((formatContext.splblockalign-2)/2 + 7)*sndInfo.channels != byteblockalign)
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
        formatContext.splblockalign = 1;
        byteblockalign = sndInfo.channels * 2;
    }
    else if(sampleFormat == FormatType::Float)
    {
        formatContext.splblockalign = 1;
        byteblockalign = sndInfo.channels * 4;
    }

    /* Figure out the OpenAL format from the file and desired sample type. */
    if(sndInfo.channels == 1)
    {
        if(sampleFormat == FormatType::Int16)
            formatContext.format = AL_FORMAT_MONO16;
        else if(sampleFormat == FormatType::Float)
            formatContext.format = AL_FORMAT_MONO_FLOAT32;
        else if(sampleFormat == FormatType::IMA4)
            formatContext.format = AL_FORMAT_MONO_IMA4;
        else if(sampleFormat == FormatType::MSADPCM)
            formatContext.format = AL_FORMAT_MONO_MSADPCM_SOFT;
    }
    else if(sndInfo.channels == 2)
    {
        if(sampleFormat == FormatType::Int16)
            formatContext.format = AL_FORMAT_STEREO16;
        else if(sampleFormat == FormatType::Float)
            formatContext.format = AL_FORMAT_STEREO_FLOAT32;
        else if(sampleFormat == FormatType::IMA4)
            formatContext.format = AL_FORMAT_STEREO_IMA4;
        else if(sampleFormat == FormatType::MSADPCM)
            formatContext.format = AL_FORMAT_STEREO_MSADPCM_SOFT;
    }
    else if(sndInfo.channels == 3)
    {
        if(sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sampleFormat == FormatType::Int16)
                formatContext.format = AL_FORMAT_BFORMAT2D_16;
            else if(sampleFormat == FormatType::Float)
                formatContext.format = AL_FORMAT_BFORMAT2D_FLOAT32;
        }
    }
    else if(sndInfo.channels == 4)
    {
        if(sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sampleFormat == FormatType::Int16)
                formatContext.format = AL_FORMAT_BFORMAT3D_16;
            else if(sampleFormat == FormatType::Float)
                formatContext.format = AL_FORMAT_BFORMAT3D_FLOAT32;
        }
    }
    if(!formatContext.format)
    {
        std::cerr << "Unsupported channel count: " << sndInfo.channels << std::endl;
        return false;
    }
    if(sndInfo.frames/formatContext.splblockalign > (sf_count_t)(INT_MAX/byteblockalign))
    {
        std::cerr << "Too many samples (" << sndInfo.frames << ")" << std::endl;
        return false;
    }

    /* Decode the whole audio file to a buffer. */
    formatContext.membuf = malloc((size_t)(sndInfo.frames / formatContext.splblockalign * byteblockalign));

    sf_count_t numFrames = 0;
    if(sampleFormat == FormatType::Int16)
    {
        numFrames = sf_readf_short(sndFile, (short*)formatContext.membuf, sndInfo.frames);
    }
    else if(sampleFormat == FormatType::Float)
    {
        numFrames = sf_readf_float(sndFile, (float*)formatContext.membuf, sndInfo.frames);
    }
    else
    {
        sf_count_t count = sndInfo.frames / formatContext.splblockalign * byteblockalign;
        numFrames = sf_read_raw(sndFile, formatContext.membuf, count);
        if(numFrames > 0)
        {
            numFrames = numFrames / byteblockalign * formatContext.splblockalign;
        }
    }

    if(numFrames < 1)
    {
        free(formatContext.membuf);
        std::cerr << "Failed to read samples (" << numFrames << ")" << std::endl;
        return false;
    }

    formatContext.numBytes = (ALsizei)(numFrames / formatContext.splblockalign * byteblockalign);

    return true;
}

void AudioSample::CreateSource()
{
    alCall(alGenSources, 1, &m_source);
    alCall(alSourcef, m_source, AL_PITCH, 1);
    alCall(alSourcef, m_source, AL_GAIN, 1.0f);
    alCall(alSource3f, m_source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, m_source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, m_source, AL_LOOPING, AL_FALSE);
    alCall(alSourcei, m_source, AL_BUFFER, m_buffer);
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
    }
}
