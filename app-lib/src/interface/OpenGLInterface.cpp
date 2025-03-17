#include "general/pch.h"

#include "OpenGLInterface.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

ae::OpenGLInterface::OpenGLInterface(Window& window)
	: Interface(window)
{
}

ae::OpenGLInterface::~OpenGLInterface()
{
}

bool ae::OpenGLInterface::CreateImpl()
{
	ImGui_ImplGlfw_InitForOpenGL(m_Window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460 core");	

	return true;
}

void ae::OpenGLInterface::DestroyImpl()
{
	ImGui_ImplOpenGL3_Shutdown();	
	ImGui_ImplGlfw_Shutdown();
}

void ae::OpenGLInterface::PrepareFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void ae::OpenGLInterface::FinishFrame()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
