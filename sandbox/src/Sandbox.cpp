#include "general/pch.h"

// Author: Rasmus Hugosson
// Date: 2025-03-17

// Description: This is a simple example program demonstrating how
// this library can be used to create a basic graphics application.

// All headers included in the library
#include "Window.h"
#include "ImGui.h"
#include "Files.h" // Not actually used in this example
#include "Vulkan.h" // Not actually used in this example
#include "OpenGL.h" // Not actually used in this example
#include "OpenAL.h" // Not actually used in this example

// Static pointer so the ImGui interface can access the Window
static ae::Window* s_pWindow = nullptr;

static void OnInterfaceUpdate()
{
	// This function is called every frame as the ImGui interface is updated
	std::shared_ptr<ae::Context> pContext = s_pWindow->GetContext().lock();

	// Check if the context is valid
	if (!pContext)
	{
		return;
	}	

	// A simple window
	ImGui::Begin("A window");

	ImGui::Text("Hello, World!");

	ImGui::End();

	// Another window with debug information
	ImGui::Begin("Another window");

	ImGui::Text("Graphics Info");

	ImGui::NewLine();

	// Graphics data can be accessed through the Window's Context
	ImGui::Text("Graphics API: %s", pContext->GetGraphicsAPI().c_str());
	ImGui::Text("Graphics API Version: %s", pContext->GetGraphicsVersion().c_str());
	ImGui::Text("Graphics Card: %s", pContext->GetGraphicsCard().c_str());
	ImGui::Text("Graphics Vendor: %s", pContext->GetGraphicsVendor().c_str());

	ImGui::NewLine();

	ImGui::Text("Frame Info");

	ImGui::NewLine();

	// Frame data can be accessed through the Window directly
	ImGui::Text("FPS: %.0f", s_pWindow->GetFps());
	ImGui::Text("Frame Time: %.2f ms", s_pWindow->GetAverageFrameTime() * 1000.0);
	ImGui::Text("Frame Duration: %.2f ms", s_pWindow->GetAverageFrameDuration() * 1000.0);

	ImGui::End();
}

int main()
{
	// We wrap the code in a try-catch block to catch any exceptions that might be thrown
	try
	{
		// Description of the window
		ae::WindowDesc windowDesc;
		windowDesc.title = "Sandbox";
		windowDesc.width = 1280;
		windowDesc.height = 720;
		windowDesc.resizable = true;
		windowDesc.minimizable = true;
		windowDesc.minimized = false;
		windowDesc.maximizable = true;
		windowDesc.maximized = false;
		windowDesc.monitor = 0;
		windowDesc.fullscreen = false;
		windowDesc.vsync = true;
		windowDesc.fps = 165; // Not actually used since vsync is enabled
		windowDesc.graphicsAPI = ae::GraphicsAPI::OPENGL;

		// We can now create a window with the descriptor
		ae::Window window(windowDesc);
		window.Create();

		// Set the static pointer to the window
		s_pWindow = &window;

		// Icons can also be loaded by specifying image paths of different sizes
		ae::IconSet icons({
			"res/icons/default16.png",
			"res/icons/default24.png",
			"res/icons/default32.png",
			"res/icons/default48.png",
			"res/icons/default64.png"
			});

		// The icon set can then be set for the window
		window.SetIconSet(icons);

		// A cursor can be created from an image path and optionally a hot spot
		ae::Cursor cursor("res/cursors/arrow_white.png", 11, 6);
		window.SetCursor(cursor);

		// The function from before must also be specified to be called every frame
		window.SetOnInterfaceUpdateCB(OnInterfaceUpdate);

		// Update loop
		while (!window.ShouldClose())
		{
			// Input events can be checked at any time
			if (window.GetKeyboard().IsKeyPressed(ae::Key::ESCAPE))
			{
				window.Close();
			}

			// We can then update the window each frame
			window.SetActive();
			window.Clear();

			// Render things

			window.Update();
		}

		s_pWindow = nullptr;

		// Clean everything up by destroying the window before termination
		window.ResetCursor();
		window.ResetIconSet();
		window.Destroy();
	}

	// If any exceptions are thrown, we catch them here	
	catch (const std::exception& e)
	{
		// We can log the exception to the console
		AE_LOG_CONSOLE_ALL(AE_ERROR, e.what());
	}
}
