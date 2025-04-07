#include "general/pch.h"

#include "VulkanContext.h"

#include <algorithm>

ae::VulkanContext::VulkanContext(Window& window)
	: Context(window), m_Surface(VK_NULL_HANDLE), m_SwapChain(VK_NULL_HANDLE), m_SwapChainImageFormat(VK_FORMAT_UNDEFINED), m_SwapChainExtent(),
	m_SwapChainImages(), m_SwapChainImageViews(), m_RenderPass(VK_NULL_HANDLE), m_SwapChainFramebuffers(), 
	m_CommandPool(VK_NULL_HANDLE), m_CommandBuffers(), m_ImageAvailableSemaphores(), m_RenderFinishedSemaphores(), m_InFlightFences(),
	m_CurrentFrame(0), m_CurrentImageIndex(0)
{
}

ae::VulkanContext::~VulkanContext()
{
}

void ae::VulkanContext::WaitForPreviousFrame()
{
	vkWaitForFences(VulkanManager::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(VulkanManager::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);
}

void ae::VulkanContext::AquireNextImage()
{
	vkAcquireNextImageKHR(
		VulkanManager::Get().GetDevice(),
		m_SwapChain,
		UINT64_MAX,
		m_ImageAvailableSemaphores[m_CurrentFrame],
		VK_NULL_HANDLE,
		&m_CurrentImageIndex
	);
}

void ae::VulkanContext::ResetCommandBuffer()
{
	vkResetCommandBuffer(m_CommandBuffers[m_CurrentImageIndex], 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	beginInfo.pInheritanceInfo = nullptr;
	if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentImageIndex], &beginInfo) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to begin recording command buffer");
	}
}

void ae::VulkanContext::BeginRenderPass()
{
	const float* clearColorData = m_Window.GetClearColor();
	VkClearValue clearColor = { clearColorData[0], clearColorData[1], clearColorData[2], clearColorData[3] };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_RenderPass;
	renderPassInfo.framebuffer = m_SwapChainFramebuffers[m_CurrentImageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChainExtent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void ae::VulkanContext::EndRenderPass()
{
	vkCmdEndRenderPass(m_CommandBuffers[m_CurrentImageIndex]);
}

void ae::VulkanContext::SubmitCommandBuffer()
{
	vkEndCommandBuffer(m_CommandBuffers[m_CurrentImageIndex]);

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentImageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(VulkanManager::Get().GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to submit Vulkan command buffer");
	}
}

void ae::VulkanContext::PresentImage()
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_SwapChain;
	presentInfo.pImageIndices = &m_CurrentImageIndex;

	VkResult result = vkQueuePresentKHR(VulkanManager::Get().GetGraphicsQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		// Could be due to window resize
		RecreateSwapChain();
	}

	else if (result != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to present Vulkan image");
	}

	m_CurrentFrame = static_cast<uint32_t>((static_cast<size_t>(m_CurrentFrame) + 1) % m_ImageAvailableSemaphores.size());
}

bool ae::VulkanContext::CreateImpl()
{
	VulkanManager::Get().AddContext(m_Window.GetDesc().title);

	CreateSurface();

	m_GraphicsAPI = "Vulkan";
	m_GraphicsVersion = VulkanManager::Get().GetVersion();
	m_GraphicsCard = VulkanManager::Get().GetRenderer();
	m_GraphicsVendor = VulkanManager::Get().GetVendor();

	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateRenderPass();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSyncObjects();

	return true;
}

void ae::VulkanContext::ActivateImpl()
{
	// Does not need to by activated
}

void ae::VulkanContext::DeactivateImpl()
{
	// Does not need to by deactivated
}

void ae::VulkanContext::DestroyImpl()
{
	vkDeviceWaitIdle(VulkanManager::Get().GetDevice());

	DestroySyncObjects();
	DestroyCommandBuffers();
	DestroyCommandPool();
	DestroyFramebuffers();
	DestroyRenderPass();
	DestroySwapChainImageViews();
	DestroySwapChain();

	DestroySurface();

	VulkanManager::Get().RemoveContext();
}

void ae::VulkanContext::CreateSurface()
{
	VkInstance instance = VulkanManager::Get().GetInstance();

	if (glfwCreateWindowSurface(instance, m_Window.GetWindow(), nullptr, &m_Surface) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan window surface");
	}

	ae::VulkanManager::Get().AddSurface(m_Surface);
}

void ae::VulkanContext::DestroySurface()
{
	ae::VulkanManager::Get().RemoveSurface(m_Surface);

	vkDestroySurfaceKHR(VulkanManager::Get().GetInstance(), m_Surface, nullptr);

	m_Surface = VK_NULL_HANDLE;
}

#undef max

void ae::VulkanContext::CreateSwapChain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		ae::VulkanManager::Get().GetPhysicalDevice(),
		m_Surface,
		&surfaceCapabilities
	);

	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

	if (surfaceCapabilities.maxImageCount > 0
		&& imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}

	uint32_t formatCount = 0;

	vkGetPhysicalDeviceSurfaceFormatsKHR(
		ae::VulkanManager::Get().GetPhysicalDevice(),
		m_Surface,
		&formatCount,
		nullptr
	);

	std::vector<VkSurfaceFormatKHR> formats(formatCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(
		ae::VulkanManager::Get().GetPhysicalDevice(),
		m_Surface,
		&formatCount,
		formats.data()
	);

	VkSurfaceFormatKHR chosenFormat = formats[0];

	for (const auto& availableFormat : formats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			chosenFormat = availableFormat;
			break;
		}
	}

	m_SwapChainImageFormat = chosenFormat.format;

	uint32_t presentModeCount = 0;

	vkGetPhysicalDeviceSurfacePresentModesKHR(
		ae::VulkanManager::Get().GetPhysicalDevice(),
		m_Surface,
		&presentModeCount,
		nullptr
	);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);

	vkGetPhysicalDeviceSurfacePresentModesKHR(
		ae::VulkanManager::Get().GetPhysicalDevice(),
		m_Surface,
		&presentModeCount,
		presentModes.data()
	);

	VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR; // guaranteed

	for (const auto& mode : presentModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			chosenPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
	}


	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		m_SwapChainExtent = surfaceCapabilities.currentExtent;
	}

	else
	{
		int width, height;
		glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);

		m_SwapChainExtent.width = std::clamp((uint32_t)width,
			surfaceCapabilities.minImageExtent.width,
			surfaceCapabilities.maxImageExtent.width);

		m_SwapChainExtent.height = std::clamp((uint32_t)height,
			surfaceCapabilities.minImageExtent.height,
			surfaceCapabilities.maxImageExtent.height);
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_SwapChainImageFormat;
	createInfo.imageColorSpace = chosenFormat.colorSpace;
	createInfo.imageExtent = m_SwapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = chosenPresentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(VulkanManager::Get().GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to create swap chain for Vulkan context");
	}

	uint32_t swapImageCount;
	vkGetSwapchainImagesKHR(VulkanManager::Get().GetDevice(), m_SwapChain, &swapImageCount, nullptr);

	m_SwapChainImages.resize(swapImageCount);
	vkGetSwapchainImagesKHR(VulkanManager::Get().GetDevice(), m_SwapChain, &swapImageCount, m_SwapChainImages.data());
}

void ae::VulkanContext::DestroySwapChain()
{
	vkDestroySwapchainKHR(VulkanManager::Get().GetDevice(), m_SwapChain, nullptr);
	m_SwapChain = VK_NULL_HANDLE;
	m_SwapChainImages.clear();
}

void ae::VulkanContext::CreateSwapChainImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_SwapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_SwapChainImageFormat;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(VulkanManager::Get().GetDevice(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
		{
			AE_THROW_VULKAN_ERROR("Failed to create image views for Vulkan context");
		}
	}
}

void ae::VulkanContext::DestroySwapChainImageViews()
{
	for (auto imageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(VulkanManager::Get().GetDevice(), imageView, nullptr);
	}

	m_SwapChainImageViews.clear();
}

void ae::VulkanContext::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(VulkanManager::Get().GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to create render pass for Vulkan context");	
	}
}

void ae::VulkanContext::DestroyRenderPass()
{
	vkDestroyRenderPass(VulkanManager::Get().GetDevice(), m_RenderPass, nullptr);

	m_RenderPass = VK_NULL_HANDLE;
}

void ae::VulkanContext::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { m_SwapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(VulkanManager::Get().GetDevice(), &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i])
			!= VK_SUCCESS)
		{
			AE_THROW_VULKAN_ERROR("Failed to create framebuffer for Vulkan context");
		}
	}
}

void ae::VulkanContext::DestroyFramebuffers()
{
	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(VulkanManager::Get().GetDevice(), framebuffer, nullptr);
	}

	m_SwapChainFramebuffers.clear();
}

void ae::VulkanContext::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = VulkanManager::Get().GetGraphicsQueueFamilyIndex();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(VulkanManager::Get().GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to create command pool for Vulkan context");
	}
}

void ae::VulkanContext::DestroyCommandPool()
{
	vkDestroyCommandPool(VulkanManager::Get().GetDevice(), m_CommandPool, nullptr);

	m_CommandPool = VK_NULL_HANDLE;
}

void ae::VulkanContext::CreateCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

	if (vkAllocateCommandBuffers(VulkanManager::Get().GetDevice(), &allocInfo, m_CommandBuffers.data())
		!= VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to allocate command buffers for Vulkan context");
	}
}

void ae::VulkanContext::DestroyCommandBuffers()
{
	vkFreeCommandBuffers(VulkanManager::Get().GetDevice(), m_CommandPool, (uint32_t)m_CommandBuffers.size(), m_CommandBuffers.data());
	
	m_CommandBuffers.clear();
}

void ae::VulkanContext::CreateSyncObjects()
{
	size_t maxFramesInFlight = 2;
	m_ImageAvailableSemaphores.resize(maxFramesInFlight);
	m_RenderFinishedSemaphores.resize(maxFramesInFlight);
	m_InFlightFences.resize(maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		if (vkCreateSemaphore(VulkanManager::Get().GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i])
			!= VK_SUCCESS ||
			vkCreateSemaphore(VulkanManager::Get().GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i])
			!= VK_SUCCESS ||
			vkCreateFence(VulkanManager::Get().GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i])
			!= VK_SUCCESS)
		{
			AE_THROW_VULKAN_ERROR("Failed to create semaphores or fences for Vulkan context");
		}
	}
}

void ae::VulkanContext::DestroySyncObjects()
{
	for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); i++)
	{
		vkDestroySemaphore(VulkanManager::Get().GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(VulkanManager::Get().GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(VulkanManager::Get().GetDevice(), m_InFlightFences[i], nullptr);
	}

	m_ImageAvailableSemaphores.clear();
	m_RenderFinishedSemaphores.clear();
	m_InFlightFences.clear();
}

void ae::VulkanContext::RecreateSwapChain()
{
	vkDeviceWaitIdle(VulkanManager::Get().GetDevice());

	DestroyFramebuffers();
	DestroyRenderPass();	
	DestroySwapChainImageViews();
	DestroySwapChain();

	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateRenderPass();
	CreateFramebuffers();

	DestroyCommandBuffers();
	CreateCommandBuffers();
}
