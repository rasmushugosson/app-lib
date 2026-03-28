#include "general/pch.h"

#include "Log.h"
#include "Vulkan.h"

uint32_t ae::VulkanFindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                  VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    AE_THROW_RUNTIME_ERROR("Failed to find suitable memory type");
}
