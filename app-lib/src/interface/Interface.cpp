#include "general/pch.h"

#include "Interface.h"

#include "ImGui.h"
#include "imgui/imgui_impl_glfw.h"

#include "Files.h"

ae::Interface::Interface(Window& window)
	: m_Window(window),  m_pContext(nullptr), m_OnInterfaceUpdate(nullptr), m_Created(false)
{
}

ae::Interface::~Interface()
{
#ifdef AE_DEBUG
	if (m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Interface was not destroyed before being deleted");
	}
#endif // AE_DEBUG
}

void ae::Interface::Create()
{
#ifdef AE_DEBUG
	if (m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to create Interface but it was already created");
		return;
	}
#endif // AE_DEBUG

    IMGUI_CHECKVERSION();
    m_pContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_pContext);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;	    // Disable changing mouse cursor

	if (!CreateImpl())
	{
		AE_THROW_GRAPHICS_ERROR("Failed to create Interface for Window");
		return;
	}

    SetImGuiStyle();
	LoadFonts();

	m_Created = true;
}

void ae::Interface::Update()
{
#ifdef AE_DEBUG
	if (!m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to update Interface but it was not created");
		return;
	}
#endif // AE_DEBUG
    if (!m_OnInterfaceUpdate)
    {
        return;
    }

    ImGui::SetCurrentContext(m_pContext);

	PrepareFrame();

	ImGui::NewFrame();

	m_OnInterfaceUpdate();

	ImGui::Render();

	FinishFrame();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ae::Interface::Destroy()
{
#ifdef AE_DEBUG
	if (!m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to destroy Interface but it was not created");
		return;
	}
#endif // AE_DEBUG
    ImGui::SetCurrentContext(m_pContext);

	DestroyImpl();

    ImGui::DestroyContext(m_pContext);

	m_Created = false;
}

void ae::Interface::SendOnKeyEvent(int key, int scancode, int action, int mods) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_KeyCallback(m_Window.GetWindow(), key, scancode, action, mods);
}

void ae::Interface::SendOnCharEvent(unsigned int c) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_CharCallback(m_Window.GetWindow(), c);
}

void ae::Interface::SendOnMouseButtonEvent(int button, int action, int mods) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_MouseButtonCallback(m_Window.GetWindow(), button, action, mods);
}   

void ae::Interface::SendOnMouseMovedEvent(double x, double y) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_CursorPosCallback(m_Window.GetWindow(), x, y);
}

void ae::Interface::SendOnMouseScrolledEvent(double x, double y) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_ScrollCallback(m_Window.GetWindow(), x, y);
}

void ae::Interface::SendOnCursorEnterEvent(int entered) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_CursorEnterCallback(m_Window.GetWindow(), entered);
}

void ae::Interface::SendOnWindowFocusEvent(int focused) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_WindowFocusCallback(m_Window.GetWindow(), focused);
}

void ae::Interface::SendOnMonitorEvent(GLFWmonitor* pMonitor, int event) const
{
    ImGui::SetCurrentContext(m_pContext);

	ImGui_ImplGlfw_MonitorCallback(pMonitor, event);
}

void ae::Interface::SetImGuiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
	
    style.WindowRounding = 16.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.TabRounding = 8.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.ScrollbarSize = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabMinSize = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabBorderSize = 0.0f;
    style.TabRounding = 8.0f;
    style.WindowPadding = ImVec2(16.0f, 16.0f);
    style.FramePadding = ImVec2(8.0f, 8.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing = 24.0f;
    style.ColumnsMinSpacing = 8.0f;
    style.ScrollbarSize = 16.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabMinSize = 8.0f;
    style.GrabRounding = 8.0f;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    style.CurveTessellationTol = 1.25f;

    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
}

void ae::Interface::LoadFonts()
{
    for (const std::string_view& fontPath : s_FontPaths)
    {
        ImGui::GetIO().Fonts->AddFontFromFileTTF(FormatDevPath(fontPath.data()).c_str(), 14.0f);
    }
}
