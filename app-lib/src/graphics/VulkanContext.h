#pragma once

#ifdef AE_VULKAN

#include <functional>
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

		FrameInfo BeginFrame();
		void EndFrame(const VkCommandBuffer* appCommandBuffers, uint32_t appCBCount, bool hasImGui);

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

		inline VkRenderPass GetImGuiStandaloneRenderPass() const { return m_ImGuiStandaloneRenderPass; }
		inline VkCommandPool GetCommandPool() const { return m_CommandPool; }

		void SetOnSwapchainRecreatedCB(const std::function<void(const VulkanResources&)>& cb);

		VulkanResources GetVulkanResources() const override;
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

		void CreateImGuiStandaloneRenderPass();
		void DestroyImGuiStandaloneRenderPass();

		void CreateImGuiOverlayRenderPass();
		void DestroyImGuiOverlayRenderPass();

		void CreateImGuiStandaloneFramebuffers();
		void DestroyImGuiStandaloneFramebuffers();

		void CreateImGuiOverlayFramebuffers();
		void DestroyImGuiOverlayFramebuffers();

		void CreateCommandPool();
		void DestroyCommandPool();

		void CreateCommandBuffers();
		void DestroyCommandBuffers();

		void CreateSyncObjects();
		void DestroySyncObjects();

		void RecreateSwapChain();

		VkCommandBuffer RecordImGuiOverlay();
		VkCommandBuffer RecordImGuiStandalone();
		VkCommandBuffer RecordTransitionToPresent();
	private:
		uint32_t m_FramesInFlight;

		VkSurfaceKHR m_Surface;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkRenderPass m_ImGuiStandaloneRenderPass;
		VkRenderPass m_ImGuiOverlayRenderPass;

		std::vector<VkFramebuffer> m_ImGuiStandaloneFramebuffers;
		std::vector<VkFramebuffer> m_ImGuiOverlayFramebuffers;

		VkCommandPool m_CommandPool;

		std::vector<VkCommandBuffer> m_ImGuiCommandBuffers;
		std::vector<VkCommandBuffer> m_TransitionCommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		uint32_t m_CurrentFrame;
		uint32_t m_CurrentImageIndex;
		bool m_NeedsResize;

		std::function<void(const VulkanResources&)> m_OnSwapchainRecreated;
	};
}

#endif // AE_VULKAN
