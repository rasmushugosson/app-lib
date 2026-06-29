#include "general/pch.h"

#include "Files.h"
#include "Log.h"

#include <filesystem>
#include <string_view>

ae::File::File() : m_Read(false), m_Populated(false) {}

ae::File::File(std::string_view path) : m_Path(AE_FILE_PATH(path)), m_Read(false), m_Populated(false) {}

ae::File::File(const std::string &path) : m_Path(AE_FILE_PATH(path)), m_Read(false), m_Populated(false) {}

ae::File &ae::File::Read()
{
    if (m_Path.empty())
    {
        AE_THROW_FILE_OPEN_ERROR("Attempted to read File with empty path. Use SetPath() to set the file path before "
                                 "calling Read()");
    }

    if (!std::filesystem::exists(m_Path))
    {
        AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load File, there is no file with the specified path '{}'", m_Path);
    }

    ReadImpl();
    m_Read = true;

    return *this;
}

ae::File &ae::File::Write()
{
    if (m_Path.empty())
    {
        AE_THROW_FILE_OPEN_ERROR("Attempted to write File with empty path. Use SetPath() to set the file path before "
                                 "calling Write()");
    }

    std::filesystem::path parent = std::filesystem::path(m_Path).parent_path();
    if (!parent.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
        if (ec)
        {
            AE_THROW_FILESYSTEM_ERROR("Failed to create directories for '{}': {}", m_Path, ec.message());
        }
    }

    WriteImpl();

    return *this;
}

void ae::File::WriteImpl()
{
    AE_THROW_RUNTIME_ERROR("This file type does not support writing");
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

void ae::File::SetPopulated(bool populated)
{
    m_Populated = populated;
}
