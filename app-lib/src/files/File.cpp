#include "Files.h"
#include "Log.h"
#include "general/pch.h"

#include <filesystem>

ae::File::File() = default;

ae::File::File(const std::string &path) : m_Path(AE_FILE_PATH(path))
{
    Load();
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

void ae::File::Load()
{
    if (!std::filesystem::exists(m_Path))
    {
        AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load File, there is no file with the specified path '{}'", m_Path);
    }

    OnLoad();
}
