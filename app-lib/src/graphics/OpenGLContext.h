#pragma once

#include "Window.h"

namespace ae
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* pWindow);
		OpenGLContext(const OpenGLContext&) = delete;
		OpenGLContext& operator=(const OpenGLContext&) = delete;
		~OpenGLContext();
	protected:
		bool CreateImpl() override;
		void ActivateImpl() override;
		void DeactivateImpl() override;
		void DestroyImpl() override;
	};
}
