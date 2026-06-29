#include "general/pch.h"

#include "Files.h"

#include <fstream>

ae::TomlFile::TomlFile() : m_pTable(std::make_unique<toml::table>()) {}

ae::TomlFile::TomlFile(const std::string &path) : TextFile(path), m_pTable(std::make_unique<toml::table>()) {}

ae::TomlFile::~TomlFile() = default;

void ae::TomlFile::ReadImpl()
{
    TextFile::ReadImpl();
    SetRead(true); // NOTE: Set read to prevent warning when getting the data

    const std::string &data = GetData();
    SetRead(false); // NOTE: Unset read again in case TOML parsing fails

    try
    {
        *m_pTable = toml::parse(data);
    }
    catch (const toml::parse_error &e)
    {
        AE_THROW_RUNTIME_ERROR("Failed to parse TOML file '{}': {}", GetPath(), e.description());
    }
}

void ae::TomlFile::WriteImpl()
{
    std::ofstream out(m_Path);

    if (!out)
    {
        AE_THROW_FILE_OPEN_ERROR("Failed to open file '{}' for writing", m_Path);
    }

    out << *m_pTable;

    if (!out)
    {
        AE_THROW_FILESYSTEM_ERROR("Failed to write TOML data to file '{}'", m_Path);
    }
}

void ae::TomlFile::SetTable(toml::table table)
{
    *m_pTable = std::move(table);
    SetPopulated(true);
}

toml::node_view<toml::node> ae::TomlFile::operator[](std::string_view key)
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access TOML data but the file has not been read or populated yet. Call Read() or SetTable() "
               "first");
    }
#endif // AE_DEBUG

    return (*m_pTable)[key];
}

toml::node_view<const toml::node> ae::TomlFile::operator[](std::string_view key) const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to access TOML data but the file has not been read or populated yet. Call Read() or SetTable() "
               "first");
    }
#endif // AE_DEBUG

    return std::as_const(*m_pTable)[key];
}

toml::table &ae::TomlFile::GetTable()
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to get TOML table but the file has not been read or populated yet. Call Read() or SetTable() "
               "first");
    }
#endif // AE_DEBUG

    return *m_pTable;
}

const toml::table &ae::TomlFile::GetTable() const
{
#ifdef AE_DEBUG
    if (!IsRead() && !IsPopulated())
    {
        AE_LOG(AE_WARNING,
               "Tried to get TOML table but the file has not been read or populated yet. Call Read() or SetTable() "
               "first");
    }
#endif // AE_DEBUG

    return *m_pTable;
}

bool ae::TomlFile::Contains(std::string_view key) const
{
    return m_pTable->contains(key);
}

size_t ae::TomlFile::Size() const
{
    return m_pTable->size();
}
