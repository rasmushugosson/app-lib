#include "general/pch.h"

#include "Files.h"

std::string ae::FormatDevPath(const std::string& path)
{
	std::ostringstream oss;

	oss << "../" << path;

	return oss.str();
}
