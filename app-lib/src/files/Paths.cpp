#include "general/pch.h"

#include "Files.h"

std::string ae::FormatDevPath(std::string_view path)
{
    return std::string(path);
}

std::string ae::FormatDevPath(const std::string &path)
{
    return FormatDevPath(std::string_view{ path });
}
