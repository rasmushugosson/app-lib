#pragma once

#ifdef AE_VULKAN

#include <vector>

#include "Vulkan.h"
#include "Window.h"

#include "VulkanManager.h"

namespace ae
{
	class VulkanContext : public Context
	{
	public:
		VulkanContext(Window& window);
		VulkanContext(const VulkanContext&) = delete;
		VulkanContext& operator=(const VulkanContext&) = delete;
		~VulkanContext();

		void WaitForPreviousFrame();
		void AquireNextImage();
		void ResetCommandBuffer();
		void BeginRenderPass();
		void EndRenderPass();
		void SubmitCommandBuffer();
		void PresentImage();

		inline VkInstance GetInstance() const { return ae::VulkanManager::Get().GetInstance(); }
		inline VkPhysicalDevice GetPhysicalDevice() const { return ae::VulkanManager::Get().GetPhysicalDevice(); }
		inline VkDevice GetDevice() const { return ae::VulkanManager::Get().GetDevice(); }
		inline VkQueue GetGraphicsQueue() const { return ae::VulkanManager::Get().GetGraphicsQueue(); }
		inline uint32_t GetGraphicsQueueFamilyIndex() const { return ae::VulkanManager::Get().GetGraphicsQueueFamilyIndex(); }

		inline VkSurfaceKHR GetSurface() const { return m_Surface; }

		inline VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		inline VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		inline VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }

		inline const std::vector<VkImage>& GetSwapChainImages() const { return m_SwapChainImages; }
		inline const std::vector<VkImageView>& GetSwapChainImageViews() const { return m_SwapChainImageViews; }

		inline VkRenderPass GetRenderPass() const { return m_RenderPass; }
		inline const std::vector<VkFramebuffer>& GetSwapChainFramebuffers() const { return m_SwapChainFramebuffers; }

		inline VkCommandPool GetCommandPool() const { return m_CommandPool; }

		inline const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_CommandBuffers; }
		inline const std::vector<VkSemaphore>& GetImageAvailableSemaphores() const { return m_ImageAvailableSemaphores; }
		inline const std::vector<VkSemaphore>& GetRenderFinishedSemaphores() const { return m_RenderFinishedSemaphores; }
		inline const std::vector<VkFence>& GetInFlightFences() const { return m_InFlightFences; }

		inline VkCommandBuffer GetCurrentCommandBuffer() const { return m_CommandBuffers[m_CurrentImageIndex]; }
		inline VkCommandBuffer GetCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }
		inline VkSemaphore GetImageAvailableSemaphore(uint32_t index) const { return m_ImageAvailableSemaphores[index]; }
		inline VkSemaphore GetRenderFinishedSemaphore(uint32_t index) const { return m_RenderFinishedSemaphores[index]; }
		inline VkFence GetInFlightFence(uint32_t index) const { return m_InFlightFences[index]; }
	protected:
		bool CreateImpl() override;
		void ActivateImpl() override;
		void DeactivateImpl() override;
		void DestroyImpl() override;
		void OnResize(uint32_t width, uint32_t height) override;
	private:
		void CreateSurface();
		void DestroySurface();

		void CreateSwapChain();
		void DestroySwapChain();

		void CreateSwapChainImageViews();
		void DestroySwapChainImageViews();

		void CreateRenderPass();	
		void DestroyRenderPass();

		void CreateFramebuffers();
		void DestroyFramebuffers();	

		void CreateCommandPool();	
		void DestroyCommandPool();

		void CreateCommandBuffers();
		void DestroyCommandBuffers();

		void CreateSyncObjects();
		void DestroySyncObjects();

		void RecreateSwapChain();
	private:
		VkSurfaceKHR m_Surface;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkCommandPool m_CommandPool;

		std::vector<VkCommandBuffer> m_CommandBuffers; // One CB per swapchain image
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		uint32_t m_CurrentFrame;
		uint32_t m_CurrentImageIndex;
		bool m_NeedsResize;
	};
}

#endif // AE_VULKAN
