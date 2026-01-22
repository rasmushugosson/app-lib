#include "general/pch.h"

#include "Files.h"
#include "Log.h"

#include <filesystem>
#include <string_view>

ae::File::File() : m_Read(false) {}

ae::File::File(std::string_view path) : m_Path(AE_FILE_PATH(path)), m_Read(false) {}

ae::File::File(const std::string &path) : m_Path(AE_FILE_PATH(path)), m_Read(false) {}

bool ae::File::Exists(std::string_view path)
{
    std::string actualPath = AE_FILE_PATH(path);

    return std::filesystem::exists(actualPath);
}

ae::File &ae::File::Read()
{
    if (!std::filesystem::exists(m_Path))
    {
        AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load File, there is no file with the specified path '{}'", m_Path);
    }

    ReadImpl();
    m_Read = true;

    return *this;
}

bool ae::File::Exists(const std::string &path)
{
    return Exists(std::string_view{ path });
}

void ae::File::SetRead(bool read)
{
    m_Read = read;
}
