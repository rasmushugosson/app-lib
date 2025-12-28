#include "general/pch.h"

#include "Log.h"
#include "OpenGL.h"
#include "Vulkan.h"
#include "Window.h"
#include "graphics/Context.h"
#include "graphics/OpenGLContext.h"
#include "graphics/VulkanContext.h"
#include "interface/Interface.h"
#include "interface/OpenGLInterface.h"
#include "interface/VulkanInterface.h"
#include "window/WindowManager.h"

#include <algorithm>
#include <utility>

ae::Window::Window(const WindowDesc &desc)
    : m_Desc(desc), m_pParent(nullptr), m_pWindow(nullptr), m_FrameTime(0.0), m_FrameDuration(0.0),
      m_AverageFrameTime(0.0), m_AverageFrameDuration(0.0), m_Fps(0.0), m_Mouse(this), m_pContext(nullptr),
      m_ClearColor{ 1.0f, 0.0f, 1.0f, 1.0f }, m_pInterface(nullptr), m_Focused(false), m_Active(false), m_Created(false)
{
#ifndef AE_VULKAN
    if (desc.graphicsAPI == ae::GraphicsAPI::VULKAN)
    {
        AE_LOG(AE_WARNING, "A window was opened with GraphicsAPI::VULKAN, but there is no Vulkan drivers present. This "
                           "will result in no window being opened");
    }
#endif // !AE_VULKAN
}

ae::Window::Window(const WindowDesc &desc, Window &parent)
    : m_Desc(desc), m_pParent(&parent), m_pWindow(nullptr), m_FrameTime(0.0), m_FrameDuration(0.0),
      m_AverageFrameTime(0.0), m_AverageFrameDuration(0.0), m_Fps(0.0), m_Mouse(this), m_pContext(nullptr),
      m_ClearColor{ 1.0f, 0.0f, 1.0f, 1.0f }, m_pInterface(nullptr), m_Focused(false), m_Active(false), m_Created(false)
{
    parent.AddChild(*this);

#ifndef AE_VULKAN
    if (desc.graphicsAPI == ae::GraphicsAPI::VULKAN)
    {
        AE_LOG(AE_WARNING, "A window was opened with GraphicsAPI::VULKAN, but there is no Vulkan drivers present. This "
                           "will result in no window being opened");
    }
#endif // !AE_VULKAN
}

ae::Window::~Window() {}

#undef CreateWindow

void ae::Window::Create()
{
    AE_LOG(AE_INFO, "Creating window...");
    AE_LOG_NEWLINE();

#ifdef AE_DEBUG
    if (m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to create Window but it is already created");
        return;
    }
#endif // AE_DEBUG

    ae::WindowManager::Get().EnsureInitialized();

    try
    {
        if (m_Desc.graphicsAPI == GraphicsAPI::OPENGL)
        {
            InitOpenGL();
        }

        else if (m_Desc.graphicsAPI == GraphicsAPI::VULKAN)
        {
#ifdef AE_VULKAN
            InitVulkan();
#else  // AE_VULKAN
            AE_THROW_RUNTIME_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE);
#endif // AE_VULKAN
        }

        glfwWindowHint(GLFW_RESIZABLE, m_Desc.resizable);
        glfwWindowHint(GLFW_MAXIMIZED, m_Desc.maximized);
        glfwWindowHint(GLFW_VISIBLE, !m_Desc.minimized);

        if (m_Desc.fullscreen)
        {
            CreateFullscreen();
        }

        else
        {
            CreateWindowed();
        }

        if (!m_pWindow)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create window");
        }

        if (m_Desc.graphicsAPI == GraphicsAPI::OPENGL)
        {
            CreateOpenGL();
        }

        else if (m_Desc.graphicsAPI == GraphicsAPI::VULKAN)
        {
#ifdef AE_VULKAN
            CreateVulkan();
#else  // AE_VULKAN
            AE_THROW_RUNTIME_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE);
#endif // AE_VULKAN
        }

        ae::WindowManager::Get().AddWindow(this);

        m_Timer.Start();
        m_FrameTimer.Start();

        AE_LOG_NEWLINE();
        AE_LOG(AE_INFO, "Window created successfully");

        m_Created = true;
        return;
    }

    catch (const std::exception &e)
    {
        AE_LOG(AE_ERROR, "{}", e.what());
    }

    catch (...)
    {
        AE_LOG(AE_ERROR, "Unexpected error thrown in Window::Create");
    }

    AE_THROW_RUNTIME_ERROR("Failed to create Window");
}

void ae::Window::Destroy()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to destroy window but it is not created");
        return;
    }
#endif // AE_DEBUG

    if (m_Desc.graphicsAPI == GraphicsAPI::OPENGL)
    {
        DestroyOpenGL();
    }

    else if (m_Desc.graphicsAPI == GraphicsAPI::VULKAN)
    {
#ifdef AE_VULKAN
        DestroyVulkan();
#else  // AE_VULKAN
        AE_THROW_RUNTIME_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE);
#endif // AE_VULKAN
    }

    ae::WindowManager::Get().RemoveWindow(this);

    ResetCursor();
    ResetIconSet();

    glfwDestroyWindow(m_pWindow);

    AE_LOG_NEWLINE();
    AE_LOG(AE_INFO, "Window destroyed successfully");

    m_Created = false;
    m_pWindow = nullptr;
}

void ae::Window::Clear() const
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to clear window but it is not created");
        return;
    }
#endif // AE_DEBUG
    if (m_Desc.graphicsAPI == GraphicsAPI::OPENGL)
    {
        GL_CHECK(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
    else if (m_Desc.graphicsAPI == GraphicsAPI::VULKAN)
    {
#ifdef AE_VULKAN
        std::shared_ptr<ae::VulkanContext> pContext = std::dynamic_pointer_cast<ae::VulkanContext>(m_pContext);
#ifdef AE_DEBUG
        if (!pContext)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create Vulkan context, context is not Vulkan");
        }
#endif // AE_DEBUG
        pContext->WaitForPreviousFrame();
        pContext->AquireNextImage();
        pContext->ResetCommandBuffer();
        pContext->BeginRenderPass();
#else  // AE_VULKAN
        AE_THROW_RUNTIME_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE);
#endif // AE_VULKAN
    }
}

#undef max

void ae::Window::Update()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to update window but it is not created");
        return;
    }
#endif // AE_DEBUG

    glfwPollEvents();

    m_pInterface->Prepare();
    m_pInterface->Update();
    m_pInterface->Finish();

    m_FrameTime = m_FrameTimer.GetElapsedTime();
    m_AverageFrameTime = (m_AverageFrameTime * 0.99) + (m_FrameTime * 0.01);

    if (m_Desc.graphicsAPI == GraphicsAPI::OPENGL)
    {
        UpdateOpenGL();
    }

    else if (m_Desc.graphicsAPI == GraphicsAPI::VULKAN)
    {
#ifdef AE_VULKAN
        UpdateVulkan();
#else  // AE_VULKAN
        AE_THROW_RUNTIME_ERROR(AE_VULKAN_NOT_FOUND_MESSAGE);
#endif // AE_VULKAN
    }

    if (!m_Desc.vsync)
    {
        double elapsedTime = m_Timer.GetElapsedTime();
        double targetTime = 1.0 / static_cast<double>(m_Desc.fps);

        if (elapsedTime < targetTime)
        {
            ae::DateTime::Wait(std::max(targetTime - elapsedTime - 0.025, 0.0));
        }

        while (m_Timer.GetElapsedTime() < targetTime)
        {
            ae::DateTime::Wait(0.0);
        }

        m_Timer.Reset();
        m_Timer.Start();
    }

    m_FrameDuration = m_FrameTimer.GetElapsedTime();
    m_AverageFrameDuration = (m_AverageFrameDuration * 0.99) + (m_FrameDuration * 0.01);
    m_Fps = 1.0 / m_AverageFrameDuration;

    m_FrameTimer.Reset();
    m_FrameTimer.Start();
}

void ae::Window::Close()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to close window but it is not created");
        return;
    }
#endif // AE_DEBUG

    for (Window *pChild : m_Children)
    {
        pChild->Close();
    }

    glfwSetWindowShouldClose(m_pWindow, true);
}

bool ae::Window::ShouldClose() const
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to check if window should close but it is not created");
        return false;
    }
#endif // AE_DEBUG

    bool childrenOpen = false;

    for (Window *pChild : m_Children)
    {
        childrenOpen = childrenOpen || !pChild->ShouldClose();
    }

    if (childrenOpen && glfwWindowShouldClose(m_pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to close Window but children are still open. This action is not valid");
        glfwSetWindowShouldClose(m_pWindow, false);
    }

    return glfwWindowShouldClose(m_pWindow);
}

void ae::Window::SetTitle(const std::string &title)
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to set window title but it is not created");
        return;
    }
#endif // AE_DEBUG
    glfwSetWindowTitle(m_pWindow, title.c_str());
}

void ae::Window::SetIconSet(const IconSet &iconSet)
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to set window icon but it is not created");
        return;
    }
#endif // AE_DEBUG
    m_IconSet = iconSet;

    if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
    {
        glfwSetWindowIcon(m_pWindow, static_cast<int>(iconSet.GetCount()), iconSet.GetImages());
    }

    AE_LOG(AE_TRACE, "Set window icon with {} images", iconSet.GetCount());
}

void ae::Window::ResetIconSet()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to reset window icon but it is not created");
        return;
    }
#endif // AE_DEBUG
    m_IconSet = IconSet();

    if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
    {
        glfwSetWindowIcon(m_pWindow, 0, nullptr);
    }
}

void ae::Window::SetCursor(const Cursor &cursor)
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to set window cursor but it is not created");
        return;
    }
#endif // AE_DEBUG
    m_CurrentCursor = cursor;

    glfwSetCursor(m_pWindow, cursor.GetCursor());
}

void ae::Window::ResetCursor()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to reset window cursor but it is not created");
        return;
    }
#endif // AE_DEBUG
    m_CurrentCursor = Cursor();

    glfwSetCursor(m_pWindow, nullptr);
}

void ae::Window::SetActive()
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to set window active but it is not created");
        return;
    }
#endif // AE_DEBUG
    m_pContext->Activate();
}

void ae::Window::SetOnInterfaceUpdateCB(const std::function<void()> &cb)
{
#ifdef AE_DEBUG
    if (!m_Created)
    {
        AE_LOG(AE_WARNING, "Tried to set interface update callback but the Window is not created");
        return;
    }
    if (!m_pInterface)
    {
        AE_LOG(AE_WARNING, "Tried to set interface update callback but the Interface for the Window is not created");
        return;
    }
#endif // AE_DEBUG
    m_pInterface->SetOnInterfaceUpdateCB(cb);
}

void ae::Window::AddChild(Window &child)
{
    m_Children.push_back(&child);
}

void ae::Window::RemoveChild(Window &child)
{
    auto it = std::ranges::find(m_Children, &child);

    if (it != m_Children.end())
    {
        m_Children.erase(it);
    }
}

GLFWmonitor *ae::Window::GetMonitor()
{
    int monitorCount = 0;
    GLFWmonitor **pMonitors = glfwGetMonitors(&monitorCount);

    if (monitorCount == 0)
    {
        AE_THROW_RUNTIME_ERROR("Failed to get monitors");
    }

    if (std::cmp_less_equal(monitorCount, m_Desc.monitor))
    {
        AE_LOG(AE_WARNING, "Invalid monitor index, using default monitor");
        m_Desc.monitor = 0;
    }

    return pMonitors[m_Desc.monitor];
}

void ae::Window::CreateWindowed()
{
    GLFWmonitor *pMonitor = GetMonitor();

    const GLFWvidmode *pVideoMode = glfwGetVideoMode(pMonitor);

    if (m_Desc.width == 0 || std::cmp_greater(m_Desc.width, pVideoMode->width) || m_Desc.height == 0 ||
        std::cmp_greater(m_Desc.height, pVideoMode->height))
    {
        AE_LOG(AE_WARNING, "Invalid window size, using default size");

        WindowDesc defaultDesc;

        m_Desc.width = defaultDesc.width;
        m_Desc.height = defaultDesc.height;
    }

    int monitorX;
    int monitorY;
    glfwGetMonitorPos(pMonitor, &monitorX, &monitorY);

    GLFWwindow *pShare = nullptr;

    if (m_pParent)
    {
        pShare = m_pParent->m_pWindow;
    }

    m_pWindow = glfwCreateWindow(static_cast<int>(m_Desc.width), static_cast<int>(m_Desc.height), m_Desc.title.c_str(),
                                 nullptr, pShare);

    if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
    {
        glfwSetWindowPos(m_pWindow, static_cast<int>(((pVideoMode->width - m_Desc.width) / 2) + monitorX),
                         static_cast<int>(((pVideoMode->height - m_Desc.height) / 2) + monitorY));
    }
}

void ae::Window::CreateFullscreen()
{
    GLFWmonitor *pMonitor = GetMonitor();

    const GLFWvidmode *pVideoMode = glfwGetVideoMode(pMonitor);
    m_Desc.width = static_cast<uint32_t>(pVideoMode->width);
    m_Desc.height = static_cast<uint32_t>(pVideoMode->height);

    GLFWwindow *pShare = nullptr;

    if (m_pParent)
    {
        pShare = m_pParent->m_pWindow;
    }

    m_pWindow = glfwCreateWindow(static_cast<int>(m_Desc.width), static_cast<int>(m_Desc.height), m_Desc.title.c_str(),
                                 pMonitor, pShare);
}

void ae::Window::InitOpenGL()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);
}

#ifdef AE_VULKAN
void ae::Window::InitVulkan()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}
#endif // AE_VULKAN

void ae::Window::CreateOpenGL()
{
    m_pContext = std::make_shared<ae::OpenGLContext>(*this);
    m_pContext->Create();
    m_Active = true;

    if (m_Desc.vsync)
    {
        GLFWmonitor *pMonitor = GetMonitor();
        const GLFWvidmode *pVideoMode = glfwGetVideoMode(pMonitor);

        m_Desc.fps = static_cast<uint32_t>(pVideoMode->refreshRate);
        m_Fps = static_cast<double>(m_Desc.fps);
        m_AverageFrameTime = 1.0 / static_cast<double>(m_Desc.fps);
        m_AverageFrameDuration = 1.0 / static_cast<double>(m_Desc.fps);

        glfwSwapInterval(1);
    }

    else
    {
        if (m_Desc.fps == 0)
        {
            AE_LOG(AE_WARNING, "Invalid FPS value for window, using default value");
            m_Desc.fps = 60;
            m_Fps = 60.0;
            m_AverageFrameTime = 1.0 / 60.0;
            m_AverageFrameDuration = 1.0 / 60.0;
        }

        glfwSwapInterval(0);
    }

    m_pInterface = std::make_unique<ae::OpenGLInterface>(*this);
    m_pInterface->Create();
}

#ifdef AE_VULKAN
void ae::Window::CreateVulkan()
{
    if (!glfwVulkanSupported())
    {
        AE_THROW_RUNTIME_ERROR("Vulkan is not supported");
    }

    m_pContext = std::make_shared<ae::VulkanContext>(*this);
    m_pContext->Create();

    if (m_Desc.vsync)
    {
        m_Desc.vsync = false;

        GLFWmonitor *pMonitor = GetMonitor();
        const GLFWvidmode *pVideoMode = glfwGetVideoMode(pMonitor);

        m_Desc.fps = static_cast<uint32_t>(pVideoMode->refreshRate);
    }

    if (m_Desc.fps == 0)
    {
        AE_LOG(AE_WARNING, "Invalid FPS value for window, using default value");
        m_Desc.fps = 60;
    }

    m_Fps = static_cast<double>(m_Desc.fps);
    m_AverageFrameTime = 1.0 / static_cast<double>(m_Desc.fps);
    m_AverageFrameDuration = 1.0 / static_cast<double>(m_Desc.fps);

    m_pInterface = std::make_unique<ae::VulkanInterface>(*this);
    m_pInterface->Create();
}
#endif // AE_VULKAN

void ae::Window::UpdateOpenGL()
{
    glfwSwapBuffers(m_pWindow);
}

#ifdef AE_VULKAN
void ae::Window::UpdateVulkan()
{
    std::shared_ptr<ae::VulkanContext> pContext = std::dynamic_pointer_cast<ae::VulkanContext>(m_pContext);
#ifdef AE_DEBUG
    if (!pContext)
    {
        AE_THROW_RUNTIME_ERROR("Failed to update Vulkan context, context is not Vulkan");
    }
#endif // AE_DEBUG
    pContext->EndRenderPass();
    pContext->SubmitCommandBuffer();
    pContext->PresentImage();
}
#endif // AE_VULKAN

void ae::Window::DestroyOpenGL()
{
    m_pInterface->Destroy();

    m_pContext->Destroy();
}

#ifdef AE_VULKAN
void ae::Window::DestroyVulkan()
{
    m_pInterface->Destroy();

    m_pContext->Destroy();
}
#endif // AE_VULKAN

void ae::Window::InitInput()
{
    uint32_t controllerIndex = 0;

    while (true)
    {
        if (ae::Controller::IsConnected(controllerIndex))
        {
            m_Controllers.emplace_back(controllerIndex);
        }

        else
        {
            break;
        }

        controllerIndex++;
    }

    AE_LOG(AE_TRACE, "Controllers connected: {}", m_Controllers.size());
}

void ae::Window::OnKey(int key, int scancode, int action, int mods)
{
    m_pInterface->SendOnKeyEvent(key, scancode, action, mods);

    if (action == GLFW_PRESS)
    {
        m_Keyboard.SetKeyPressed(static_cast<int32_t>(key), true);

        if (m_OnKeyPressed)
        {
            m_OnKeyPressed(static_cast<int32_t>(key));
        }
    }

    else if (action == GLFW_RELEASE)
    {
        m_Keyboard.SetKeyPressed(static_cast<int32_t>(key), false);

        if (m_OnKeyReleased)
        {
            m_OnKeyReleased(static_cast<int32_t>(key));
        }
    }
}

void ae::Window::OnChar(unsigned int c)
{
    m_pInterface->SendOnCharEvent(c);

    m_Keyboard.SetKeyTyped(static_cast<int32_t>(c));

    if (m_OnKeyTyped)
    {
        m_OnKeyTyped(static_cast<int32_t>(c));
    }
}

void ae::Window::OnMouseButton(int button, int action, int mods)
{
    m_pInterface->SendOnMouseButtonEvent(button, action, mods);

    if (action == GLFW_PRESS)
    {
        m_Mouse.SetPressed(static_cast<int32_t>(button), true);

        if (m_OnMouseButtonPressed)
        {
            m_OnMouseButtonPressed(button);
        }
    }
    else if (action == GLFW_RELEASE)
    {
        m_Mouse.SetPressed(static_cast<int32_t>(button), false);

        if (m_OnMouseButtonReleased)
        {
            m_OnMouseButtonReleased(button);
        }
    }
}

void ae::Window::OnMouseMoved(double x, double y)
{
    m_pInterface->SendOnMouseMovedEvent(x, y);

    m_Mouse.SetMoved(static_cast<float>(x), static_cast<float>(y));

    if (m_OnMouseMoved)
    {
        m_OnMouseMoved(static_cast<float>(x), static_cast<float>(y));
    }
}

void ae::Window::OnMouseScrolled(double x, double y)
{
    m_pInterface->SendOnMouseScrolledEvent(x, y);

    m_Mouse.SetScrolled(static_cast<float>(x), static_cast<float>(y));

    if (m_OnMouseScrolled)
    {
        m_OnMouseScrolled(static_cast<float>(x), static_cast<float>(y));
    }
}

void ae::Window::OnMouseEntered(int entered)
{
    m_pInterface->SendOnCursorEnterEvent(entered);

    m_Mouse.SetEntered(static_cast<bool>(entered));

    if (m_OnMouseEntered)
    {
        m_OnMouseEntered();
    }

    if (entered)
    {
        glfwSetCursor(m_pWindow, m_CurrentCursor.GetCursor());
    }
}

void ae::Window::OnWindowResize(uint32_t width, uint32_t height)
{
    m_Desc.width = width;
    m_Desc.height = height;

    if (m_pContext)
    {
        m_pContext->OnResize(width, height);
    }

    if (m_OnWindowResize)
    {
        m_OnWindowResize(width, height);
    }
}

void ae::Window::OnWindowMinimalized()
{
    if (m_OnWindowMinimized)
    {
        m_OnWindowMinimized();
    }
}

void ae::Window::OnWindowMaximalized()
{
    if (m_OnWindowMaximized)
    {
        m_OnWindowMaximized();
    }
}

void ae::Window::OnWindowRestored()
{
    if (m_OnWindowRestored)
    {
        m_OnWindowRestored();
    }
}

void ae::Window::OnWindowMoved(uint32_t x, uint32_t y)
{
    if (m_OnWindowMoved)
    {
        m_OnWindowMoved(x, y);
    }
}

void ae::Window::OnWindowFocused(int focused)
{
    m_pInterface->SendOnWindowFocusEvent(focused);

    m_Focused = static_cast<bool>(focused);

    if (m_OnWindowFocused)
    {
        m_OnWindowFocused(focused);
    }
}

void ae::Window::OnMonitor(GLFWmonitor *pMonitor, int event)
{
    m_pInterface->SendOnMonitorEvent(pMonitor, event);

    if (m_OnMonitorConnected)
    {
        m_OnMonitorConnected();
    }
}

void ae::Window::Deactivate()
{
    m_pContext->Deactivate();

    m_Active = false;
}
