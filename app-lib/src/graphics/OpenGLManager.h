#pragma once

#include "OpenGL.h"

namespace ae
{
	class OpenGLManager
	{
	private:
		OpenGLManager();
	public:
		OpenGLManager(const OpenGLManager&) = delete;
		OpenGLManager& operator=(const OpenGLManager&) = delete;
		~OpenGLManager();

		static inline OpenGLManager& Get()
		{
			return m_Instance;
		}

		void AddContext();
		void RemoveContext();

		inline uint32_t GetContextCount() const { return m_ContextCount; }

		inline const std::string& GetVersion() const { return m_Version; }
		inline const std::string& GetRenderer() const { return m_Renderer; }
		inline const std::string& GetVendor() const { return m_Vendor; }
	private:
		uint32_t m_ContextCount = 0;
		std::string m_Version;
		std::string m_Renderer;
		std::string m_Vendor;
	private:
		static OpenGLManager m_Instance;
	};
}
