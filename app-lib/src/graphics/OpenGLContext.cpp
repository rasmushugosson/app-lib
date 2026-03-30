#include "general/pch.h"

#include "OpenGLContext.h"
#include "OpenGLManager.h"
#include "window/WindowManager.h"

ae::OpenGLContext::OpenGLContext(Window &window) : Context(window) {}

bool ae::OpenGLContext::CreateImpl()
{
    glfwMakeContextCurrent(m_Window.GetWindow());

    OpenGLManager::Get().AddContext();

    m_GraphicsAPI = "OpenGL";
    m_GraphicsVersion = OpenGLManager::Get().GetVersion();
    m_GraphicsCard = OpenGLManager::Get().GetRenderer();
    m_GraphicsVendor = OpenGLManager::Get().GetVendor();

    return true;
}

void ae::OpenGLContext::ActivateImpl()
{
    WindowManager::Get().DeactivateAllWindows();

    glfwMakeContextCurrent(m_Window.GetWindow());
}

void ae::OpenGLContext::DeactivateImpl() {}

void ae::OpenGLContext::DestroyImpl()
{
    OpenGLManager::Get().RemoveContext();

    m_GraphicsAPI = "Undefined";
    m_GraphicsVersion = "Undefined";
    m_GraphicsCard = "Undefined";
    m_GraphicsVendor = "Undefined";
}

#if defined(AE_VULKAN) && defined(AE_DEBUG)
ae::VulkanResources ae::OpenGLContext::GetVulkanResources() const
{
    AE_THROW_RUNTIME_ERROR(
        "Tried to get Vulkan resources from Context, but your created Window uses OpenGL as its graphics API");
    return {};
}
#endif
