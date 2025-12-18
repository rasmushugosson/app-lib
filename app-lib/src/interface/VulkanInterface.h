#pragma once

#ifdef AE_VULKAN

#include "Vulkan.h"
#include "DearImGui.h"

#include "Interface.h"

namespace ae
{
	class VulkanInterface : public Interface
	{
	public:
		VulkanInterface(Window& window);
		VulkanInterface(const VulkanInterface&) = delete;
		VulkanInterface& operator=(const VulkanInterface&) = delete;
		virtual ~VulkanInterface();
	protected:
		virtual bool CreateImpl() override;
		virtual void DestroyImpl() override;

		virtual void PrepareImpl() override;
		virtual void FinishImpl() override;
	private:
		void CreateDescriptorPool();
		void DestroyDescriptorPool();

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	private:
		VkDescriptorPool m_DescriptorPool;
	};
}

#endif // AE_VULKAN
