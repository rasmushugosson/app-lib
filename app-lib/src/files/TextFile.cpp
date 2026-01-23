#include "general/pch.h"

#include "Files.h"

#include <utility>

ae::TextFile::TextFile()  = default;

ae::TextFile::TextFile(const std::string &path) : File(path) {}

void ae::TextFile::ReadImpl()
{
    FILE *file = fopen(m_Path.c_str(), "rb");

    if (!file)
    {
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0)
    {
        fclose(file);
        return;
    }

    m_Data.resize(size);

    size_t read = fread(m_Data.data(), 1, size, file);
    fclose(file);

    if (std::cmp_not_equal(read, size))
    {
        // If partial read
        m_Data.resize(read);
    }
}
