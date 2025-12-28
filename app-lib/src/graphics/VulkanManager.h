#pragma once

#ifdef AE_VULKAN

#include "Vulkan.h"

namespace ae
{
	class VulkanManager
	{
	private:
		VulkanManager();
	public:
		VulkanManager(const VulkanManager&) = delete;
		VulkanManager& operator=(const VulkanManager&) = delete;
		~VulkanManager();

		static inline VulkanManager& Get()
		{
			static VulkanManager instance;
			return instance;
		}

		void AddContext(const std::string& name);
		void RemoveContext();

		void AddSurface(VkSurfaceKHR surface);
		void RemoveSurface(VkSurfaceKHR surface);

		inline const std::string& GetVersion() const { return m_Version; }
		inline const std::string& GetRenderer() const { return m_Renderer; }
		inline const std::string& GetVendor() const { return m_Vendor; }

		inline VkInstance GetInstance() const { return m_VulkanInstance; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }	
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		inline uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
	private:
		void CreateInstance(const std::string& name);
		void DestroyInstance();

		void CreateDevices();
		void RecreateDevices();
		void DestroyDevices();

		void FindPhysicalDevice();
		void ResetPhysicalDevice();

		void CreateLogicalDevice();
		void DestroyLogicalDevice();

		void SetGraphicsQueue();
		void ResetGraphicsQueue();

		std::vector<const char*> GetRequiredExtensions();
		bool IsValidationLayersSupported();

		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool IsDeviceExtensionsSupported(VkPhysicalDevice device);
		int32_t FindQueueFamilies(VkPhysicalDevice device);
		double RateDevice(VkPhysicalDevice device);

		void FindDeviceData();
		void ResetDeviceData();
	private:
		uint32_t m_ContextCount = 0;
		std::string m_Version;
		std::string m_Renderer;
		std::string m_Vendor;

		VkInstance m_VulkanInstance;
		std::vector<VkSurfaceKHR> m_Surfaces;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;

		VkQueue m_GraphicsQueue;
		uint32_t m_GraphicsQueueFamilyIndex;
	private:
#ifdef AE_DEBUG
		static constexpr std::array<const char*, 1> s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#else // AE_DEBUG
		static constexpr std::array<const char*, 0> s_ValidationLayers = {};
#endif // AE_DEBUG

		static constexpr std::array<const char*, 1> s_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}

#endif // AE_VULKAN
