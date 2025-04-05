#pragma once

#include <functional>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "ImGui.h"

#include "Window.h"

namespace ae
{
	class Interface
	{
	public:
		Interface(Window& window);
		Interface(const Interface&) = delete;
		Interface& operator=(const Interface&) = delete;
		virtual ~Interface();

		void Create();

		void Prepare();
		void Update();
		void Finish();

		void Destroy();

		void SendOnKeyEvent(int key, int scancode, int action, int mods) const;

		void SendOnCharEvent(unsigned int c) const;

		void SendOnMouseButtonEvent(int button, int action, int mods) const;

		void SendOnMouseMovedEvent(double x, double y) const;

		void SendOnMouseScrolledEvent(double x, double y) const;

		void SendOnCursorEnterEvent(int entered) const;

		void SendOnWindowFocusEvent(int focused) const;

		void SendOnMonitorEvent(GLFWmonitor* pMonitor, int event) const;

		inline void SetOnInterfaceUpdateCB(const std::function<void()>& cb) { m_OnInterfaceUpdate = cb; }
	protected:
		virtual bool CreateImpl() = 0;
		virtual void DestroyImpl() = 0;

		virtual void PrepareImpl() = 0;
		virtual void FinishImpl() = 0;
	private:
		void SetImGuiStyle();
		void LoadFonts();
	protected:
		Window& m_Window;
		ImGuiContext* m_pContext;
		std::function<void()> m_OnInterfaceUpdate;
		bool m_Created;
	private:
		static constexpr std::array<std::string_view, 1> s_FontPaths =
		{
			"res/fonts/Ubuntu-Medium.ttf",
		};
	};
}
