#pragma once

#define AE_VULKAN_NOT_FOUND_MESSAGE                                                                                    \
    "Vulkan is not linked and therefore cannot be used. Make sure it is installed and set as a PATH variable. Check "  \
    "premake5.lua file for further info"

#ifdef AE_VULKAN

#include "Log.h"

#include <vulkan/vulkan.h>

#ifndef AE_DIST

namespace ae
{
void VulkanCheckResult(VkResult result, const std::string &call, const std::string &file, uint32_t line);

#define VK_CHECK(x)                                                                                                    \
    VkResult err = x;                                                                                                  \
    ae::VulkanCheckResult(err, #x, __FILE__, __LINE__)
} // namespace ae

#else

#define VK_CHECK(x) x

#endif // AE_DIST

#else // AE_VULKAN

#ifndef AE_DIST

#define VK_CHECK(x) throw ae::VulkanError(__FILE__, __LINE__, std::ostringstream() << AE_VULKAN_NOT_FOUND_MESSAGE)

#else

#define VK_CHECK(x)

#endif // AE_DIST

#endif // AE_VULKAN
