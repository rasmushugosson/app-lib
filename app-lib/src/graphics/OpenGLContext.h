#pragma once

#include "Window.h"

namespace ae
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(Window& window);
		OpenGLContext(const OpenGLContext&) = delete;
		OpenGLContext& operator=(const OpenGLContext&) = delete;
		~OpenGLContext() = default;

#if defined(AE_VULKAN) && defined(AE_DEBUG)
		VulkanResources GetVulkanResources() const override;
#endif
	protected:
		bool CreateImpl() override;
		void ActivateImpl() override;
		void DeactivateImpl() override;
		void DestroyImpl() override;
	};
}
