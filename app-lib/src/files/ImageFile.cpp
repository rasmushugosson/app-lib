#include "general/pch.h"

#include "Files.h"

#include <filesystem>

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

template<typename T>
ae::ImageFile<T>::ImageFile(const std::string& path)
	: m_Path(AE_FILE_PATH(path))
	, m_Data()
	, m_Width(0)
	, m_Height(0)
	, m_Channels(0)
{
	Load();
}

template<typename T>
ae::ImageFile<T>::ImageFile(T* pData, uint32_t width, uint32_t height, uint32_t channels)
	: m_Path("")
	, m_Data(pData)
	, m_Width(width)
	, m_Height(height)
	, m_Channels(channels)
{
}

template<typename T>
ae::ImageFile<T>::ImageFile(const std::vector<T>& data, uint32_t width, uint32_t height, uint32_t channels)
	: m_Path("")
	, m_Data()
	, m_Width(width)
	, m_Height(height)
	, m_Channels(channels)
{
	m_Data.resize(data.size());
	memcpy(m_Data.data(), data.data(), data.size() * sizeof(T));
}

template<typename T>
ae::ImageFile<T>::~ImageFile()
{
}

template<typename T>
void ae::ImageFile<T>::Export(const std::string& path) const
{
	if (m_Data.empty())
	{
		return;
	}

	std::string extension = path.substr(path.find_last_of('.') + 1);

	if (extension == "png")
	{
		stbi_write_png(path.c_str(), m_Width, m_Height, m_Channels, m_Data.data(), m_Width * m_Channels * sizeof(T));
	}

	else if (extension == "jpg" || extension == "jpeg")
	{
		stbi_write_jpg(path.c_str(), m_Width, m_Height, m_Channels, m_Data.data(), 100);
	}

	else if (extension == "bmp")
	{
		stbi_write_bmp(path.c_str(), m_Width, m_Height, m_Channels, m_Data.data());
	}

	else if (extension == "tga")
	{
		stbi_write_tga(path.c_str(), m_Width, m_Height, m_Channels, m_Data.data());
	}

	else if (extension == "hdr")
	{
		if constexpr (std::is_same<T, uint8_t>::value)
		{
			std::vector<float> data(m_Data.size());

			for (size_t i = 0; i < m_Data.size(); i++)
			{
				data[i] = static_cast<float>(m_Data[i]) / 255.0f;
			}

			stbi_write_hdr(path.c_str(), m_Width, m_Height, m_Channels, data.data());
		}
		else
		{
			stbi_write_hdr(path.c_str(), m_Width, m_Height, m_Channels, reinterpret_cast<float*>(m_Data.data()));
		}
	}
}

template<typename T>
void ae::ImageFile<T>::Load()
{
	if (!std::filesystem::exists(m_Path))
	{
		AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load ImageFile, file was not found");
	}

	if constexpr (std::is_same<T, uint8_t>::value)
	{
		stbi_uc* pData = stbi_load(m_Path.c_str(), reinterpret_cast<int*>(&m_Width), reinterpret_cast<int*>(&m_Height), reinterpret_cast<int*>(&m_Channels), 0);
		
		if (pData)
		{
			m_Data.resize(m_Width * m_Height * m_Channels);
			memcpy(m_Data.data(), pData, m_Data.size());
			stbi_image_free(pData);
		}

		else
		{
			AE_THROW_RUNTIME_ERROR("Failed to load ImageFile, the file was not a valid image file. Valid image formats are PNG, JPG, JPEG, BMP, TGA, HDR");
		}
	}

	else if constexpr (std::is_same<T, float>::value)
	{
		float* pData = stbi_loadf(m_Path.c_str(), reinterpret_cast<int*>(&m_Width), reinterpret_cast<int*>(&m_Height), reinterpret_cast<int*>(&m_Channels), 0);

		if (pData)
		{
			m_Data.resize(m_Width * m_Height * m_Channels);
			memcpy(m_Data.data(), pData, m_Data.size());
			stbi_image_free(pData);
		}

		else
		{
			AE_THROW_RUNTIME_ERROR("Failed to load ImageFile, the file was not a valid image file. Valid image formats are PNG, JPG, JPEG, BMP, TGA, HDR");
		}
	}
}
