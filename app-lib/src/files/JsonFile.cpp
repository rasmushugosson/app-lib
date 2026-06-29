#include "general/pch.h"

#include "Files.h"

#include <fstream>

ae::JsonFile::JsonFile() : m_pJson(std::make_unique<nlohmann::json>()) {}

ae::JsonFile::JsonFile(const std::string &path) : TextFile(path), m_pJson(std::make_unique<nlohmann::json>()) {}

ae::JsonFile::~JsonFile() = default;

void ae::JsonFile::ReadImpl()
{
    TextFile::ReadImpl();
    SetRead(true); // NOTE: Set read to prevent warning when using getting the data

    const std::string &data = GetData();
    SetRead(false); // NOTE: Unset read again in case JSON parsing fails

    try
    {
        *m_pJson = nlohmann::json::parse(data);
    }

    catch (const nlohmann::json::parse_error &e)
    {
        AE_THROW_RUNTIME_ERROR("Failed to parse JSON file '{}': {}", GetPath(), e.what());
    }
}

void ae::JsonFile::WriteImpl()
{
    std::ofstream out(m_Path);

    if (!out)
    {
        AE_THROW_FILE_OPEN_ERROR("Failed to open file '{}' for writing", m_Path);
    }

    out << m_pJson->dump(4);

    if (!out)
    {
        AE_THROW_FILESYSTEM_ERROR("Failed to write JSON data to file '{}'", m_Path);
    }
}

void ae::JsonFile::SetJson(nlohmann::json json)
{
    *m_pJson = std::move(json);
    SetPopulated(true);
}

nlohmann::json &ae::JsonFile::operator[](const std::string &key)
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[key];
}

const nlohmann::json &ae::JsonFile::operator[](const std::string &key) const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[key];
}

nlohmann::json &ae::JsonFile::operator[](const char *key)
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[key];
}

const nlohmann::json &ae::JsonFile::operator[](const char *key) const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[key];
}

nlohmann::json &ae::JsonFile::operator[](size_t index)
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[index];
}

const nlohmann::json &ae::JsonFile::operator[](size_t index) const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access JSON data but the file has not been read or populated yet. Call Read() or SetJson() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pJson)[index];
}

nlohmann::json &ae::JsonFile::GetJson()
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to get JSON but the file has not been read or populated yet. Call Read() or SetJson() first");
    }
#endif // AE_DEBUG

    return *m_pJson;
}

const nlohmann::json &ae::JsonFile::GetJson() const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to get JSON but the file has not been read or populated yet. Call Read() or SetJson() first");
    }
#endif // AE_DEBUG

    return *m_pJson;
}

bool ae::JsonFile::IsObject() const
{
    return m_pJson->is_object();
}

bool ae::JsonFile::IsArray() const
{
    return m_pJson->is_array();
}

bool ae::JsonFile::IsString() const
{
    return m_pJson->is_string();
}

bool ae::JsonFile::IsNumber() const
{
    return m_pJson->is_number();
}

bool ae::JsonFile::IsBoolean() const
{
    return m_pJson->is_boolean();
}

bool ae::JsonFile::IsNull() const
{
    return m_pJson->is_null();
}

bool ae::JsonFile::Contains(const std::string &key) const
{
    return m_pJson->contains(key);
}

size_t ae::JsonFile::Size() const
{
    return m_pJson->size();
}

std::string ae::JsonFile::Dump(int indent) const
{
    return m_pJson->dump(indent);
}
