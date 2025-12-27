#include "general/pch.h"

#include "Files.h"
#include "stb_vorbis.h"

#include <filesystem>

ae::AudioFile::AudioFile(const std::string &path) : File(path), m_SampleRate(0), m_Channels(0), m_Samples(0) {}

void ae::AudioFile::ReadImpl()
{
    int32_t error = 0;

    stb_vorbis *pVorbis = stb_vorbis_open_filename(m_Path.c_str(), &error, nullptr);

    if (!pVorbis)
    {
        AE_THROW_RUNTIME_ERROR("Failed to load AudioFile, the file is not a valid audio file. Valid audio formats are "
                               "OGG, WAV, FLAC, MP3");
    }

    stb_vorbis_info info = stb_vorbis_get_info(pVorbis);
    m_SampleRate = info.sample_rate;
    m_Channels = info.channels;
    m_Samples = stb_vorbis_stream_length_in_samples(pVorbis);

    int samples = static_cast<int>(m_Samples * m_Channels);
    m_Data.resize(samples);

    int32_t read =
        stb_vorbis_get_samples_short_interleaved(pVorbis, static_cast<int>(m_Channels), m_Data.data(), samples);

    if (read != samples)
    {
        AE_THROW_RUNTIME_ERROR("Failed to load AudioFile, all samples could not be read");
    }

    stb_vorbis_close(pVorbis);
}

ae::AudioFileStream::AudioFileStream(const std::string &path, uint32_t chunkSamples)
    : File(path), m_pVorbis(nullptr), m_SampleRate(0), m_Channels(0), m_Samples(0), m_ChunkSamples(chunkSamples),
      m_EndReached(false)
{
}

ae::AudioFileStream::~AudioFileStream()
{
    Close();
}

std::vector<short> ae::AudioFileStream::GetNextChunkData()
{
    if (m_EndReached)
    {
        return {};
    }

    int samples = static_cast<int>(m_ChunkSamples * m_Channels);
    std::vector<short> data(samples);

    int32_t read =
        stb_vorbis_get_samples_short_interleaved(m_pVorbis, static_cast<int>(m_Channels), data.data(), samples);

    if (read < samples)
    {
        m_EndReached = true;
    }

    data.resize(static_cast<size_t>(read) * m_Channels);

    return data;
}

void ae::AudioFileStream::Restart()
{
    stb_vorbis_seek_start(m_pVorbis);
}

void ae::AudioFileStream::ReadImpl()
{
    int32_t error = 0;
    m_pVorbis = stb_vorbis_open_filename(m_Path.c_str(), &error, nullptr);

    if (!m_pVorbis)
    {
        AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load AudioFileStream, the file is not a valid audio file. Valid audio "
                                      "formats are OGG, WAV, FLAC, MP3");
    }

    stb_vorbis_info info = stb_vorbis_get_info(m_pVorbis);
    m_SampleRate = info.sample_rate;
    m_Channels = info.channels;
    m_Samples = stb_vorbis_stream_length_in_samples(m_pVorbis);
}

void ae::AudioFileStream::Close()
{
    if (m_pVorbis)
    {
        stb_vorbis_close(m_pVorbis);
        m_pVorbis = nullptr;
    }
}
