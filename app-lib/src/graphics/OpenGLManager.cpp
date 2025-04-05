#include "general/pch.h"

#include "OpenGLManager.h"

ae::OpenGLManager ae::OpenGLManager::m_Instance;

ae::OpenGLManager::OpenGLManager()
	: m_ContextCount(0), m_Version("None"), m_Renderer("None"), m_Vendor("None")
{
}

ae::OpenGLManager::~OpenGLManager()
{
}

void ae::OpenGLManager::AddContext()
{
	if (m_ContextCount == 0)
	{
		if (glewInit() != GLEW_OK)
		{
			AE_THROW_GRAPHICS_ERROR("Failed to initialize GLEW when first graphics Context was created");
			return;
		}

		m_Version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		m_Renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		m_Vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

		AE_LOG_CONSOLE(AE_INFO, "OpenGL Version: " << m_Version);
		AE_LOG_CONSOLE(AE_INFO, "OpenGL Renderer: " << m_Renderer);
		AE_LOG_CONSOLE(AE_INFO, "OpenGL Vendor: " << m_Vendor);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}

	m_ContextCount++;
}

void ae::OpenGLManager::RemoveContext()
{
	m_ContextCount--;
}
