#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <type_traits>

struct stb_vorbis;

namespace ae
{
	std::string FormatDevPath(const std::string& path);

#ifdef AE_DIST
#define AE_FILE_PATH(path) path
#else 
#define AE_FILE_PATH(path) FormatDevPath(path)
#endif // AE_DIST

	template<typename T>
	class ImageFile
	{
		static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, float>::value, "ImageFile only supports uint8_t or float types");
	public:
		ImageFile(const std::string& path);
		ImageFile(T* pData, uint32_t width, uint32_t height, uint32_t channels);
		ImageFile(const std::vector<T>& data, uint32_t width, uint32_t height, uint32_t channels);
		~ImageFile();
		
		void Export(const std::string& path) const;	

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
		inline uint32_t GetChannels() const { return m_Channels; }
		const std::vector<T>& GetData() const { return m_Data; }
	private:
		void Load();
	private:
		std::string m_Path;
		std::vector<T> m_Data;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};

	class AudioFile
	{
	public:
		AudioFile(const std::string& path);
		~AudioFile();

		inline uint32_t GetSampleRate() const { return m_SampleRate; }
		inline uint32_t GetChannels() const { return m_Channels; }
		inline uint32_t GetSamples() const { return m_Samples; }
		inline double GetDuration() const { return static_cast<double>(m_Samples) / static_cast<double>(m_SampleRate); }

		const std::vector<short>& GetData() const { return m_Data; }
	private:
		void Load();
	private:	
		std::string m_Path;
		std::vector<short> m_Data;
		uint32_t m_SampleRate;
		uint32_t m_Channels;
		uint32_t m_Samples;
	};

	class AudioFileStream
	{
	public:
		AudioFileStream(const std::string& path, uint32_t chunkSamples);
		~AudioFileStream();

		std::vector<short> GetNextChunkData();
		void Restart();

		inline bool EndReached() const { return m_EndReached; }

		inline uint32_t GetSampleRate() const { return m_SampleRate; }
		inline uint32_t GetChannels() const { return m_Channels; }
		inline uint32_t GetSamples() const { return m_Samples; }
		inline uint32_t GetChunkSamples() const { return m_ChunkSamples; }
		inline double GetDuration() const { return static_cast<double>(m_Samples) / static_cast<double>(m_SampleRate); }
		inline double GetChunkDuration() const { return static_cast<double>(m_ChunkSamples) / static_cast<double>(m_SampleRate); }
	private:
		void Load();
		void Unload();
	private:
		std::string m_Path;
		stb_vorbis* m_pVorbis;
		uint32_t m_SampleRate;
		uint32_t m_Channels;
		uint32_t m_Samples;
		uint32_t m_ChunkSamples;
		bool m_EndReached;
	};
}
