#include "general/pch.h"

#include "Files.h"

#include <utility>

ae::BinaryFile::BinaryFile() = default;

ae::BinaryFile::BinaryFile(const std::string &path) : File(path) {}

void ae::BinaryFile::ReadImpl()
{
    FILE *file = fopen(m_Path.c_str(), "rb");
    if (!file)
    {
        AE_THROW_FILE_OPEN_ERROR("Failed to open file '{}' for reading", m_Path);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0)
    {
        fclose(file);
        AE_THROW_FILESYSTEM_ERROR("Failed to determine the size of file '{}'", m_Path);
    }

    m_Data.resize(size);

    size_t read = fread(m_Data.data(), 1, size, file);
    fclose(file);

    if (std::cmp_not_equal(read, size))
    {
        m_Data.resize(read);
    }
}

void ae::BinaryFile::WriteImpl()
{
    FILE *file = fopen(m_Path.c_str(), "wb");

    if (!file)
    {
        AE_THROW_FILE_OPEN_ERROR("Failed to open file '{}' for writing", m_Path);
    }

    size_t written = fwrite(m_Data.data(), 1, m_Data.size(), file);
    fclose(file);

    if (std::cmp_not_equal(written, m_Data.size()))
    {
        AE_THROW_FILESYSTEM_ERROR("Failed to write all data to file '{}'", m_Path);
    }
}

void ae::BinaryFile::SetData(std::vector<uint8_t> data)
{
    m_Data = std::move(data);
    SetPopulated(true);
}
