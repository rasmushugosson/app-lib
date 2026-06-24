#pragma once

#define AE_VULKAN_NOT_FOUND_MESSAGE                                                                                    \
    "Vulkan is not linked and therefore cannot be used. Make sure it is installed and set as a PATH variable. Check "  \
    "premake5.lua file for further info"

#ifdef AE_VULKAN

#include "Log.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vk_mem_alloc.h>

namespace ae
{
struct VulkanResources
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t graphicsQueueFamilyIndex;

    VkFormat swapchainFormat;
    VkExtent2D swapchainExtent;
    uint32_t imageCount;
    const VkImage *swapchainImages;
    const VkImageView *swapchainImageViews;
};

uint32_t VulkanFindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void VulkanCheckResult(VkResult result, const std::string &call, const std::string &file, uint32_t line);
} // namespace ae

#define VK_CHECK(x)                                                                                                    \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        ae::VulkanCheckResult(err, #x, __FILE__, __LINE__);                                                            \
    }

#else // AE_VULKAN

#include "Log.h"

#define VK_CHECK(x) AE_THROW_VULKAN_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE)

#endif // AE_VULKAN
