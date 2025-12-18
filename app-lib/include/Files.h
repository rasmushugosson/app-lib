#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

struct stb_vorbis;

namespace ae
{
std::string FormatDevPath(std::string_view path);
std::string FormatDevPath(const std::string &path);

#ifdef AE_DIST
#define AE_FILE_PATH(path) std::string(path)
#else
#define AE_FILE_PATH(path) FormatDevPath(path)
#endif // AE_DIST

class File
{
  public:
    File();
    File(const std::string &path);
    virtual ~File() = default;

    [[nodiscard]] inline const std::string &GetPath() const
    {
        return m_Path;
    }

    static bool Exists(std::string_view path);
    static bool Exists(const std::string &path);

  protected:
    void Load();

    virtual void OnLoad() = 0;

  protected:
    std::string m_Path;
};

class TextFile : public File
{
  public:
    TextFile(const std::string &path);
    ~TextFile() override = default;

    [[nodiscard]] inline const std::string &GetData() const
    {
        return m_Data;
    }

  protected:
    void OnLoad() override;

  private:
    std::string m_Data;
};

class BinaryFile : public File
{
  public:
    BinaryFile(const std::string &path);
    ~BinaryFile() override = default;

    [[nodiscard]] inline const std::vector<uint8_t> &GetData() const
    {
        return m_Data;
    }

  protected:
    void OnLoad() override;

  private:
    std::vector<uint8_t> m_Data;
};

template <typename T> class ImageFile : public File
{
    static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, float>,
                  "ImageFile only supports uint8_t or float types");

  public:
    ImageFile(const std::string &path);
    ImageFile(T *pData, uint32_t width, uint32_t height, uint32_t channels);
    ImageFile(const std::vector<T> &data, uint32_t width, uint32_t height, uint32_t channels);
    ~ImageFile() override = default;

    void Export(const std::string &path) const;

    [[nodiscard]] inline uint32_t GetWidth() const
    {
        return m_Width;
    }
    [[nodiscard]] inline uint32_t GetHeight() const
    {
        return m_Height;
    }
    [[nodiscard]] inline uint32_t GetChannels() const
    {
        return m_Channels;
    }
    const std::vector<T> &GetData() const
    {
        return m_Data;
    }

  private:
    void OnLoad() override;

  private:
    std::string m_Path;
    std::vector<T> m_Data;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_Channels;
};

class AudioFile : public File
{
  public:
    AudioFile(const std::string &path);
    ~AudioFile() override = default;

    [[nodiscard]] inline uint32_t GetSampleRate() const
    {
        return m_SampleRate;
    }
    [[nodiscard]] inline uint32_t GetChannels() const
    {
        return m_Channels;
    }
    [[nodiscard]] inline uint32_t GetSamples() const
    {
        return m_Samples;
    }
    [[nodiscard]] inline double GetDuration() const
    {
        return static_cast<double>(m_Samples) / static_cast<double>(m_SampleRate);
    }

    [[nodiscard]] const std::vector<short> &GetData() const
    {
        return m_Data;
    }

  private:
    void OnLoad() override;

  private:
    std::vector<short> m_Data;
    uint32_t m_SampleRate;
    uint32_t m_Channels;
    uint32_t m_Samples;
};

class AudioFileStream : public File
{
  public:
    AudioFileStream(const std::string &path, uint32_t chunkSamples);
    ~AudioFileStream() override;

    std::vector<short> GetNextChunkData();
    void Restart();

    [[nodiscard]] inline bool EndReached() const
    {
        return m_EndReached;
    }

    [[nodiscard]] inline uint32_t GetSampleRate() const
    {
        return m_SampleRate;
    }
    [[nodiscard]] inline uint32_t GetChannels() const
    {
        return m_Channels;
    }
    [[nodiscard]] inline uint32_t GetSamples() const
    {
        return m_Samples;
    }
    [[nodiscard]] inline uint32_t GetChunkSamples() const
    {
        return m_ChunkSamples;
    }
    [[nodiscard]] inline double GetDuration() const
    {
        return static_cast<double>(m_Samples) / static_cast<double>(m_SampleRate);
    }
    [[nodiscard]] inline double GetChunkDuration() const
    {
        return static_cast<double>(m_ChunkSamples) / static_cast<double>(m_SampleRate);
    }

  private:
    void OnLoad() override;
    void Unload();

  private:
    stb_vorbis *m_pVorbis;
    uint32_t m_SampleRate;
    uint32_t m_Channels;
    uint32_t m_Samples;
    uint32_t m_ChunkSamples;
    bool m_EndReached;
};
} // namespace ae
