#include "general/pch.h"

#include "Files.h"
#include "stb_image.h"
#include "stb_image_write.h"

template <typename T> ae::ImageFile<T>::ImageFile() : File(), m_Data(), m_Width(0), m_Height(0), m_Channels(0) {}

template <typename T>
ae::ImageFile<T>::ImageFile(const std::string &path) : File(path), m_Data(), m_Width(0), m_Height(0), m_Channels(0)
{
    Read();
}

template <typename T>
ae::ImageFile<T>::ImageFile(T *pData, uint32_t width, uint32_t height, uint32_t channels)
    : File(), m_Data(), m_Width(width), m_Height(height), m_Channels(channels)
{
    size_t dataSize = static_cast<size_t>(width) * height * channels;
    m_Data.resize(dataSize);
    memcpy(m_Data.data(), pData, dataSize * sizeof(T));
    SetPopulated(true);
}

template <typename T>
ae::ImageFile<T>::ImageFile(const std::vector<T> &data, uint32_t width, uint32_t height, uint32_t channels)
    : File(), m_Data(), m_Width(width), m_Height(height), m_Channels(channels)
{
    m_Data.resize(data.size());
    memcpy(m_Data.data(), data.data(), data.size() * sizeof(T));
    SetPopulated(true);
}

template <typename T> void ae::ImageFile<T>::WriteImpl()
{
    if (m_Data.empty())
    {
        AE_THROW_RUNTIME_ERROR("Failed to write ImageFile '{}', there is no image data to write", m_Path);
    }

    std::string extension = m_Path.substr(m_Path.find_last_of('.') + 1);

    int result = 0;

    if (extension == "png")
    {
        result = stbi_write_png(m_Path.c_str(), m_Width, m_Height, m_Channels, m_Data.data(),
                                static_cast<int>(sizeof(T) * m_Width * m_Channels));
    }

    else if (extension == "jpg" || extension == "jpeg")
    {
        result = stbi_write_jpg(m_Path.c_str(), m_Width, m_Height, m_Channels, m_Data.data(), 100);
    }

    else if (extension == "bmp")
    {
        result = stbi_write_bmp(m_Path.c_str(), m_Width, m_Height, m_Channels, m_Data.data());
    }

    else if (extension == "tga")
    {
        result = stbi_write_tga(m_Path.c_str(), m_Width, m_Height, m_Channels, m_Data.data());
    }

    else if (extension == "hdr")
    {
        if constexpr (std::is_same_v<T, uint8_t>)
        {
            std::vector<float> data(m_Data.size());

            for (size_t i = 0; i < m_Data.size(); i++)
            {
                data[i] = static_cast<float>(m_Data[i]) / 255.0f;
            }

            result = stbi_write_hdr(m_Path.c_str(), m_Width, m_Height, m_Channels, data.data());
        }
        else
        {
            result = stbi_write_hdr(m_Path.c_str(), m_Width, m_Height, m_Channels, const_cast<float *>(m_Data.data()));
        }
    }

    else
    {
        AE_THROW_INVALID_ARGUMENT("Failed to write ImageFile '{}', unsupported file extension '{}'. Valid image formats "
                                  "are PNG, JPG, JPEG, BMP, TGA, HDR",
                                  m_Path, extension);
    }

    if (result == 0)
    {
        AE_THROW_FILE_OPEN_ERROR("Failed to write ImageFile to '{}'", m_Path);
    }
}

template <typename T> void ae::ImageFile<T>::ReadImpl()
{
    if constexpr (std::is_same_v<T, uint8_t>)
    {
        stbi_uc *pData = stbi_load(m_Path.c_str(), reinterpret_cast<int *>(&m_Width),
                                   reinterpret_cast<int *>(&m_Height), reinterpret_cast<int *>(&m_Channels), 0);

        if (pData)
        {
            m_Data.resize(m_Width * m_Height * m_Channels);
            memcpy(m_Data.data(), pData, m_Data.size() * sizeof(T));
            stbi_image_free(pData);
        }

        else
        {
            AE_THROW_RUNTIME_ERROR("Failed to load ImageFile, the file is not a valid image file. Valid image formats "
                                   "are PNG, JPG, JPEG, BMP, TGA, HDR");
        }
    }

    else if constexpr (std::is_same_v<T, float>)
    {
        float *pData = stbi_loadf(m_Path.c_str(), reinterpret_cast<int *>(&m_Width), reinterpret_cast<int *>(&m_Height),
                                  reinterpret_cast<int *>(&m_Channels), 0);

        if (pData)
        {
            m_Data.resize(m_Width * m_Height * m_Channels);
            memcpy(m_Data.data(), pData, m_Data.size() * sizeof(T));
            stbi_image_free(pData);
        }

        else
        {
            AE_THROW_RUNTIME_ERROR("Failed to load ImageFile, the file is not a valid image file. Valid image formats "
                                   "are PNG, JPG, JPEG, BMP, TGA, HDR");
        }
    }
}

// Explicit template instantiations
template class ae::ImageFile<uint8_t>;
template class ae::ImageFile<float>;
