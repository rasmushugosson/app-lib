#include "general/pch.h"

#ifdef AE_VULKAN

#include "OpenGL.h"
#include "VulkanManager.h"

#include <algorithm>
#include <utility>

ae::VulkanManager ae::VulkanManager::m_Instance;

ae::VulkanManager::VulkanManager()
    : m_ContextCount(0), m_Version("None"), m_Renderer("None"), m_Vendor("None"), m_VulkanInstance(VK_NULL_HANDLE),
      m_PhysicalDevice(VK_NULL_HANDLE), m_Device(VK_NULL_HANDLE), m_GraphicsQueue(VK_NULL_HANDLE),
      m_GraphicsQueueFamilyIndex(0)
{
}

ae::VulkanManager::~VulkanManager() = default;

void ae::VulkanManager::AddContext(const std::string &name)
{
    if (m_ContextCount == 0)
    {
        CreateInstance(name);
    }

    m_ContextCount++;
}

void ae::VulkanManager::RemoveContext()
{
    m_ContextCount--;

    if (m_ContextCount == 0)
    {
        DestroyInstance();
    }
}

void ae::VulkanManager::AddSurface(VkSurfaceKHR surface)
{
    m_Surfaces.push_back(surface);

    if (m_ContextCount == 1)
    {
        CreateDevices();
        SetGraphicsQueue();

        FindDeviceData();
    }

    else
    {
        if (!IsDeviceSuitable(m_PhysicalDevice))
        {
            RecreateDevices();
            SetGraphicsQueue();

            FindDeviceData();
        }
    }
}

void ae::VulkanManager::RemoveSurface(VkSurfaceKHR surface)
{
    auto it = std::ranges::find(m_Surfaces, surface);

    if (it != m_Surfaces.end())
    {
        m_Surfaces.erase(it);
    }

    if (m_Surfaces.empty())
    {
        ResetGraphicsQueue();
        DestroyDevices();

        ResetDeviceData();
    }
}

void ae::VulkanManager::CreateInstance(const std::string &name)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char *> extensions = GetRequiredExtensions();

    if (!IsValidationLayersSupported())
    {
        AE_THROW_RUNTIME_ERROR("Failed to find required validation layers");
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (!s_ValidationLayers.empty())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }

    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create Vulkan instance");
    }
}

void ae::VulkanManager::DestroyInstance()
{
    vkDestroyInstance(m_VulkanInstance, nullptr);

    m_VulkanInstance = VK_NULL_HANDLE;
}

void ae::VulkanManager::CreateDevices()
{
    FindPhysicalDevice();
    CreateLogicalDevice();
}

void ae::VulkanManager::RecreateDevices()
{
    DestroyDevices();
    CreateDevices();
}

void ae::VulkanManager::DestroyDevices()
{
    DestroyLogicalDevice();
    ResetPhysicalDevice();
}

void ae::VulkanManager::FindPhysicalDevice()
{
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    double bestScore = -1000000.0;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        AE_THROW_RUNTIME_ERROR("Failed to find available Vulkan physical devices");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        double score = RateDevice(device);

        if (score > bestScore)
        {
            bestScore = score;
            physicalDevice = device;
        }
    }

    if (bestScore < 0.0)
    {
        AE_THROW_RUNTIME_ERROR("Failed to find suitable Vulkan physical device");
    }

    m_PhysicalDevice = physicalDevice;
}

void ae::VulkanManager::ResetPhysicalDevice()
{
    m_PhysicalDevice = VK_NULL_HANDLE;
}

void ae::VulkanManager::CreateLogicalDevice()
{
    m_GraphicsQueueFamilyIndex = FindQueueFamilies(m_PhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

    if (!s_ValidationLayers.empty())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }

    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create Vulkan logical device");
    }
}

void ae::VulkanManager::DestroyLogicalDevice()
{
    vkDestroyDevice(m_Device, nullptr);

    m_Device = VK_NULL_HANDLE;
}

void ae::VulkanManager::SetGraphicsQueue()
{
    vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
}

void ae::VulkanManager::ResetGraphicsQueue()
{
    m_GraphicsQueue = VK_NULL_HANDLE;
    m_GraphicsQueueFamilyIndex = 0;
}

std::vector<const char *> ae::VulkanManager::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

bool ae::VulkanManager::IsValidationLayersSupported()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : s_ValidationLayers)
    {

        bool found = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    return true;
}

bool ae::VulkanManager::IsDeviceSuitable(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueProps.data());

    if (!IsDeviceExtensionsSupported(device))
    {
        return false;
    }

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            bool supportsAllSurfaces = true;

            for (auto &surf : m_Surfaces)
            {
                VkBool32 presentSupport = VK_FALSE;

                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surf, &presentSupport);

                if (!presentSupport)
                {
                    supportsAllSurfaces = false;
                    break;
                }
            }

            if (supportsAllSurfaces)
            {
                return true;
            }
        }
    }

    return false;
}

bool ae::VulkanManager::IsDeviceExtensionsSupported(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    for (const char *extension : s_DeviceExtensions)
    {
        bool found = false;

        for (const auto &extensionProperties : availableExtensions)
        {
            if (strcmp(extension, extensionProperties.extensionName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    return true;
}

int32_t ae::VulkanManager::FindQueueFamilies(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (int32_t i = 0; std::cmp_less(i, queueFamilyCount); i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            bool supportsAllSurfaces = true;

            for (auto &surface : m_Surfaces)
            {
                VkBool32 canPresent = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &canPresent);
                if (!canPresent)
                {
                    supportsAllSurfaces = false;
                    break;
                }
            }

            if (supportsAllSurfaces)
            {
                return i; // This queue can do graphics + present to all surfaces
            }
        }
    }

    return -1; // No suitable queue found
}

double ae::VulkanManager::RateDevice(VkPhysicalDevice device)
{
    int32_t queueFamilyIndices = FindQueueFamilies(device);

    if (queueFamilyIndices < 0)
    {
        return -1.0; // Not suitable if no graphics queue
    }

    if (!IsDeviceSuitable(device))
    {
        return -1.0; // Not suitable if not all surfaces are supported
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    double score = 0.0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) // Not integrated
    {
        score += 1000;
    }

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    uint64_t totalMemory = 0;

    for (uint32_t i = 0; i < memProperties.memoryHeapCount; i++)
    {
        totalMemory += memProperties.memoryHeaps[i].size;
    }

    score += static_cast<double>(totalMemory) / (1024.0 * 1024.0);

    return score;
}

void ae::VulkanManager::FindDeviceData()
{
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);

    m_Renderer = props.deviceName;

    std::string apiVersion = std::to_string(VK_VERSION_MAJOR(props.apiVersion)) + "." +
                             std::to_string(VK_VERSION_MINOR(props.apiVersion)) + "." +
                             std::to_string(VK_VERSION_PATCH(props.apiVersion));

    std::string vendorString = "Unknown";

    switch (props.vendorID)
    {
    case 0x1002:
        vendorString = "AMD";
        break;
    case 0x10DE:
        vendorString = "NVIDIA";
        break;
    case 0x8086:
        vendorString = "Intel";
        break;
    default:
        break;
    }

    std::string deviceVersion = std::to_string(VK_VERSION_MAJOR(props.driverVersion)) + "." +
                                std::to_string(VK_VERSION_MINOR(props.driverVersion)) + "." +
                                std::to_string(VK_VERSION_PATCH(props.driverVersion));

    m_Version = apiVersion + " " + vendorString + " " + deviceVersion;

    m_Vendor = vendorString;

    AE_LOG(AE_TRACE, "Vulkan Version: {}", m_Version);
    AE_LOG(AE_TRACE, "Vulkan Renderer: {}", m_Renderer);
    AE_LOG(AE_TRACE, "Vulkan Vendor: {}", m_Vendor);
}

void ae::VulkanManager::ResetDeviceData()
{
    m_Version = "None";
    m_Renderer = "None";
    m_Vendor = "None";
}

#endif // AE_VULKAN
