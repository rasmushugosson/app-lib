#pragma once

#ifdef AE_VULKAN

#include <vulkan/vulkan.h>

#include "Log.h"

namespace ae
{
	class VulkanError : public std::runtime_error
	{
	public:
		explicit VulkanError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Vulkan error", file, line, message))
		{
		}
	};
}

#define AE_THROW_VULKAN_ERROR(m) throw ae::VulkanError(__FILE__, __LINE__, std::ostringstream() << m)

#ifndef AE_DIST

namespace ae
{
	void VulkanCheckResult(VkResult result, const char* function, const char* file, uint32_t line);

	#define VK_CHECK(x) VkResult err = x; ae::VulkanCheckResult(err, #x, __FILE__, __LINE__)
}

#else

#define VK_CHECK(x) x

#endif // AE_DIST

#else // AE_VULKAN

#ifndef AE_DIST

#define AE_VULKAN_NOT_FOUND_MESSAGE "Vulkan is not linked and therefore cannot be used. Make sure it is installed and set as a PATH variable. Check premake5.lua file for further info"
#define VK_CHECK(x) throw ae::VulkanError(__FILE__, __LINE__, std::ostringstream() << AE_VULKAN_NOT_FOUND_MESSAGE)

#else

#define VK_CHECK(x)

#endif // AE_DIST

#endif // AE_VULKAN
