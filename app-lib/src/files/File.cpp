#include "general/pch.h"

#include "Files.h"
#include "Log.h"

#include <filesystem>
#include <string_view>

ae::File::File() : m_Read(false) {}

ae::File::File(std::string_view path) : m_Path(AE_FILE_PATH(path)), m_Read(false) {}

ae::File::File(const std::string &path) : m_Path(AE_FILE_PATH(path)), m_Read(false) {}

ae::File &ae::File::Read()
{
    if (m_Path.empty())
    {
        AE_LOG(AE_WARNING, "Attempted to read File with empty path. Use SetPath() to set the file path before calling "
                           "Read()");
        return *this;
    }

    if (!std::filesystem::exists(m_Path))
    {
        AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load File, there is no file with the specified path '{}'", m_Path);
    }

    ReadImpl();
    m_Read = true;

    return *this;
}

void ae::File::SetPath(std::string_view path)
{
    if (m_Read)
    {
        AE_LOG(AE_WARNING, "Attempted to set path on File that has already been read. SetPath() cannot be used after "
                           "Read() has been called");
        return;
    }

    m_Path = AE_FILE_PATH(path);
}

void ae::File::SetPath(const std::string &path)
{
    SetPath(std::string_view{ path });
}

bool ae::File::Exists(std::string_view path)
{
    std::string actualPath = AE_FILE_PATH(path);

    return std::filesystem::exists(actualPath);
}

bool ae::File::Exists(const std::string &path)
{
    return Exists(std::string_view{ path });
}

void ae::File::SetRead(bool read)
{
    m_Read = read;
}
