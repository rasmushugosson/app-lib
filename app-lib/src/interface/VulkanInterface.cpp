#include "general/pch.h"

#include "VulkanInterface.h"

#include "graphics/VulkanContext.h"

#include "imgui/imgui_impl_glfw.h"	
#include "imgui/imgui_impl_vulkan.h"

ae::VulkanInterface::VulkanInterface(Window& window)
	: Interface(window), m_DescriptorPool(VK_NULL_HANDLE)
{
}

ae::VulkanInterface::~VulkanInterface()
{
}

bool ae::VulkanInterface::CreateImpl()
{
	std::shared_ptr<Context> pContext = m_Window.GetContext().lock();

#ifdef AE_DEBUG
	if (!pContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is null");
		return false;
	}
#endif // AE_DEBUG

	std::shared_ptr<VulkanContext> pVulkanContext = std::dynamic_pointer_cast<VulkanContext>(pContext);

#ifdef AE_DEBUG
	if (!pVulkanContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is not Vulkan");
		return false;
	}
#endif // AE_DEBUG

	CreateDescriptorPool();

	ImGui_ImplGlfw_InitForVulkan(m_Window.GetWindow(), true);

	ImGui_ImplVulkan_InitInfo init_info = {};

	init_info.Instance = VulkanManager::Get().GetInstance();
	init_info.PhysicalDevice = VulkanManager::Get().GetPhysicalDevice();
	init_info.Device = VulkanManager::Get().GetDevice();
	init_info.QueueFamily = VulkanManager::Get().GetGraphicsQueueFamilyIndex();
	init_info.Queue = pVulkanContext->GetGraphicsQueue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = m_DescriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = static_cast<uint32_t>(pVulkanContext->GetSwapChainImages().size());
	init_info.CheckVkResultFn = nullptr;

	// Copy font atlas to GPU
	ImGui_ImplVulkan_Init(&init_info, pVulkanContext->GetRenderPass());

	VkCommandBuffer fontCmd = BeginSingleTimeCommands();

	ImGui_ImplVulkan_CreateFontsTexture(fontCmd);
	EndSingleTimeCommands(fontCmd);

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	return true;
}

void ae::VulkanInterface::DestroyImpl()
{
	vkDeviceWaitIdle(VulkanManager::Get().GetDevice());

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	DestroyDescriptorPool();
}

void ae::VulkanInterface::PrepareImpl()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void ae::VulkanInterface::FinishImpl()
{
	std::shared_ptr<Context> pContext = m_Window.GetContext().lock();
#ifdef AE_DEBUG
	if (!pContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is null");
	}
#endif // AE_DEBUG

	std::shared_ptr<VulkanContext> pVulkanContext = std::dynamic_pointer_cast<VulkanContext>(pContext);

#ifdef AE_DEBUG
	if (!pVulkanContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is not Vulkan");
	}
#endif // AE_DEBUG

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), pVulkanContext->GetCurrentCommandBuffer());
}

void ae::VulkanInterface::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 11> poolSizes = {
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000 * static_cast<uint32_t>(poolSizes.size());
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();

	if (vkCreateDescriptorPool(VulkanManager::Get().GetDevice(), &poolInfo, nullptr, &m_DescriptorPool)
		!= VK_SUCCESS)
	{
		AE_THROW_VULKAN_ERROR("Failed to create ImGui descriptor pool for Vulkan");
	}
}

void ae::VulkanInterface::DestroyDescriptorPool()
{
	vkDestroyDescriptorPool(VulkanManager::Get().GetDevice(), m_DescriptorPool, nullptr);

	m_DescriptorPool = VK_NULL_HANDLE;
}

VkCommandBuffer ae::VulkanInterface::BeginSingleTimeCommands()
{
	std::shared_ptr<Context> pContext = m_Window.GetContext().lock();

#ifdef AE_DEBUG
	if (!pContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is null");
		return VK_NULL_HANDLE;
	}
#endif // AE_DEBUG

	std::shared_ptr<VulkanContext> pVulkanContext = std::dynamic_pointer_cast<VulkanContext>(pContext);

#ifdef AE_DEBUG
	if (!pVulkanContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is not Vulkan");
		return VK_NULL_HANDLE;
	}
#endif // AE_DEBUG

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pVulkanContext->GetCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmd;
	vkAllocateCommandBuffers(VulkanManager::Get().GetDevice(), &allocInfo, &cmd);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd, &beginInfo);

	return cmd;
}

void ae::VulkanInterface::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	std::shared_ptr<Context> pContext = m_Window.GetContext().lock();

#ifdef AE_DEBUG
	if (!pContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is null");
		return;
	}
#endif // AE_DEBUG

	std::shared_ptr<VulkanContext> pVulkanContext = std::dynamic_pointer_cast<VulkanContext>(pContext);

#ifdef AE_DEBUG
	if (!pVulkanContext)
	{
		AE_THROW_VULKAN_ERROR("Failed to create Vulkan interface, context is not Vulkan");
		return;
	}
#endif // AE_DEBUG

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(VulkanManager::Get().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(VulkanManager::Get().GetGraphicsQueue());

	vkFreeCommandBuffers(VulkanManager::Get().GetDevice(), pVulkanContext->GetCommandPool(), 1, &commandBuffer);
}
