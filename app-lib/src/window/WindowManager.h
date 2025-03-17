#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <vector>
#include <unordered_map>

#include "Log.h"
#include "Window.h"

namespace ae
{
	class GLFWError : public std::runtime_error
	{
	public:
		explicit GLFWError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("GLFW error", file, line, message))
		{
		}
	};

#define AE_THROW_GLFW_ERROR(m) throw ae::GLFWError(__FILE__, __LINE__, std::ostringstream() << m);

	class WindowManager
	{
	public:
		WindowManager();
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
		~WindowManager();

		inline static WindowManager& Get() { return m_Instance; }

		void AddWindow(Window* window);
		void RemoveWindow(Window* window);

		void RecordKey(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
		void RecordChar(GLFWwindow* pWindow, unsigned int c);

		void RecordMouseButton(GLFWwindow* pWindow, int button, int action, int mods);
		void RecordMouseMoved(GLFWwindow* pWindow, double x, double y);
		void RecordMouseScrolled(GLFWwindow* pWindow, double x, double y);
		void RecordMouseEntered(GLFWwindow* pWindow, int entered);

		void RecordWindowResize(GLFWwindow* pWindow, uint32_t width, uint32_t height);
		void RecordWindowMinimalized(GLFWwindow* pWindow);
		void RecordWindowMaximalized(GLFWwindow* pWindow);
		void RecordWindowRestored(GLFWwindow* pWindow);
		void RecordWindowMoved(GLFWwindow* pWindow, uint32_t x, uint32_t y);
		void RecordWindowFocused(GLFWwindow* pWindow, int focused);

		void RecordMonitor(GLFWmonitor* pMonitor, int event);

		void DeactivateAllWindows();
	private:
		void Init();
		void Terminate();
	private:
		std::vector<Window*> m_Windows;
		std::unordered_map<GLFWwindow*, uint32_t> m_WindowMap;

		static WindowManager m_Instance;
	};
}
