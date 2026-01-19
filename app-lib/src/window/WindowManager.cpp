#include "general/pch.h"

#include "Log.h"
#include "window/WindowManager.h"

#include <GLFW/glfw3.h>

static void GLFWErrorCallback(int error, const char *description)
{
    AE_THROW_RUNTIME_ERROR("GLFW error\n\nCode: {}\nDescription: {}", error, description);
}

static void GLFWKeyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    ae::WindowManager::Get().RecordKey(pWindow, key, scancode, action, mods);
}

static void GLFWMouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{
    ae::WindowManager::Get().RecordMouseButton(pWindow, button, action, mods);
}

static void GLFWMouseMovedCallback(GLFWwindow *pWindow, double x, double y)
{
    ae::WindowManager::Get().RecordMouseMoved(pWindow, x, y);
}

static void GLFWMouseScrolledCallback(GLFWwindow *pWindow, double x, double y)
{
    ae::WindowManager::Get().RecordMouseScrolled(pWindow, x, y);
}

static void GLFWCursorEnterCallback(GLFWwindow *pWindow, int entered)
{
    ae::WindowManager::Get().RecordMouseEntered(pWindow, entered);
}

static void GLFWWindowSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    ae::WindowManager::Get().RecordWindowResize(pWindow, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

static void GLFWWindowMinimalizeCallback(GLFWwindow *pWindow, int minimized)
{
    if (minimized == GLFW_TRUE)
    {
        ae::WindowManager::Get().RecordWindowMinimalized(pWindow);
    }

    else
    {
        ae::WindowManager::Get().RecordWindowRestored(pWindow);
    }
}

static void GLFWWindowMaximalizeCallback(GLFWwindow *pWindow, int maximized)
{
    if (maximized == GLFW_TRUE)
    {
        ae::WindowManager::Get().RecordWindowMaximalized(pWindow);
    }

    else
    {
        ae::WindowManager::Get().RecordWindowRestored(pWindow);
    }
}

static void GLFWWindowMoveCallback(GLFWwindow *pWindow, int x, int y)
{
    ae::WindowManager::Get().RecordWindowMoved(pWindow, static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}

static void GLFWWindowFocusCallback(GLFWwindow *pWindow, int focused)
{
    ae::WindowManager::Get().RecordWindowFocused(pWindow, focused);
}

static void GLFWMonitorCallback(GLFWmonitor *pMonitor, int event)
{
    ae::WindowManager::Get().RecordMonitor(pMonitor, event);
}

static void GLFWFramebufferSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    ae::WindowManager::Get().RecordFramebufferResize(pWindow, static_cast<uint32_t>(width),
                                                     static_cast<uint32_t>(height));
}

static void GLFWContentScaleCallback(GLFWwindow *pWindow, float xScale, float yScale)
{
    ae::WindowManager::Get().RecordContentScaleChanged(pWindow, xScale, yScale);
}

static void GLFWDropCallback(GLFWwindow *pWindow, int count, const char **paths)
{
    ae::WindowManager::Get().RecordFileDrop(pWindow, count, paths);
}

static void GLFWWindowCloseCallback(GLFWwindow *pWindow)
{
    ae::WindowManager::Get().RecordWindowClose(pWindow);
}

static void GLFWJoystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        ae::WindowManager::Get().RecordControllerConnected(jid);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        ae::WindowManager::Get().RecordControllerDisconnected(jid);
    }
}

ae::WindowManager::WindowManager() : m_Initialized(false) {}

ae::WindowManager::~WindowManager()
{
    if (m_Initialized)
    {
        try
        {
            Terminate();
        }

        catch (...)
        {
            std::fputs("Unexpected exception thrown in WindowManager destructor", stderr);
        }
    }
}

void ae::WindowManager::AddWindow(Window *window)
{
    m_WindowMap[window->GetWindow()] = static_cast<uint32_t>(m_Windows.size());
    m_Windows.push_back(window);

    if (window->GetDesc().type != WindowType::HEADLESS)
    {
        glfwSetKeyCallback(window->GetWindow(), GLFWKeyCallback);

        glfwSetMouseButtonCallback(window->GetWindow(), GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(window->GetWindow(), GLFWMouseMovedCallback);
        glfwSetScrollCallback(window->GetWindow(), GLFWMouseScrolledCallback);
        glfwSetCursorEnterCallback(window->GetWindow(), GLFWCursorEnterCallback);

        glfwSetWindowSizeCallback(window->GetWindow(), GLFWWindowSizeCallback);
        glfwSetWindowIconifyCallback(window->GetWindow(), GLFWWindowMinimalizeCallback);
        glfwSetWindowMaximizeCallback(window->GetWindow(), GLFWWindowMaximalizeCallback);
        glfwSetWindowPosCallback(window->GetWindow(), GLFWWindowMoveCallback);
        glfwSetWindowFocusCallback(window->GetWindow(), GLFWWindowFocusCallback);

        glfwSetFramebufferSizeCallback(window->GetWindow(), GLFWFramebufferSizeCallback);
        glfwSetWindowContentScaleCallback(window->GetWindow(), GLFWContentScaleCallback);
        glfwSetDropCallback(window->GetWindow(), GLFWDropCallback);
        glfwSetWindowCloseCallback(window->GetWindow(), GLFWWindowCloseCallback);

        glfwSetMonitorCallback(GLFWMonitorCallback);
        glfwSetJoystickCallback(GLFWJoystickCallback);
    }

    AE_LOG(AE_TRACE, "Window added to manager");
}

void ae::WindowManager::RemoveWindow(Window *window)
{
    if (window->GetDesc().type != WindowType::HEADLESS)
    {
        glfwSetKeyCallback(window->GetWindow(), nullptr);

        glfwSetMouseButtonCallback(window->GetWindow(), nullptr);
        glfwSetCursorPosCallback(window->GetWindow(), nullptr);
        glfwSetScrollCallback(window->GetWindow(), nullptr);
        glfwSetCursorEnterCallback(window->GetWindow(), nullptr);

        glfwSetWindowSizeCallback(window->GetWindow(), nullptr);
        glfwSetWindowIconifyCallback(window->GetWindow(), nullptr);
        glfwSetWindowMaximizeCallback(window->GetWindow(), nullptr);
        glfwSetWindowPosCallback(window->GetWindow(), nullptr);
        glfwSetWindowFocusCallback(window->GetWindow(), nullptr);

        glfwSetFramebufferSizeCallback(window->GetWindow(), nullptr);
        glfwSetWindowContentScaleCallback(window->GetWindow(), nullptr);
        glfwSetDropCallback(window->GetWindow(), nullptr);
        glfwSetWindowCloseCallback(window->GetWindow(), nullptr);
    }

    uint32_t index = m_WindowMap[window->GetWindow()];

    for (auto &pair : m_WindowMap)
    {
        // Shift the index of all windows that have higher index than the removed window
        if (pair.second > index)
        {
            pair.second--;
        }
    }

    m_Windows.erase(m_Windows.begin() + index);

    m_WindowMap.erase(window->GetWindow());

    AE_LOG(AE_TRACE, "Window removed from manager");
}

void ae::WindowManager::Init()
{
    if (!glfwInit())
    {
        AE_THROW_RUNTIME_ERROR("Failed to initialize GLFW");
    }

    glfwSetErrorCallback(GLFWErrorCallback);
    m_Initialized = true;
}

void ae::WindowManager::Terminate()
{
    glfwTerminate();
    m_Initialized = false;
}

void ae::WindowManager::EnsureInitialized()
{
    if (!m_Initialized)
    {
        Init();
    }
}

void ae::WindowManager::RecordKey(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnKey(key, scancode, action, mods);
}

void ae::WindowManager::RecordMouseButton(GLFWwindow *pWindow, int button, int action, int mods)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnMouseButton(button, action, mods);
}

void ae::WindowManager::RecordMouseMoved(GLFWwindow *pWindow, double x, double y)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnMouseMoved(x, y);
}

void ae::WindowManager::RecordMouseScrolled(GLFWwindow *pWindow, double x, double y)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnMouseScrolled(x, y);
}

void ae::WindowManager::RecordMouseEntered(GLFWwindow *pWindow, int entered)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnMouseEntered(entered);
}

void ae::WindowManager::RecordWindowResize(GLFWwindow *pWindow, uint32_t width, uint32_t height)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowResize(width, height);
}

void ae::WindowManager::RecordWindowFocused(GLFWwindow *pWindow, int focused)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowFocused(focused);
}

void ae::WindowManager::RecordWindowMinimalized(GLFWwindow *pWindow)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowMinimalized();
}

void ae::WindowManager::RecordWindowMaximalized(GLFWwindow *pWindow)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowMaximalized();
}

void ae::WindowManager::RecordWindowRestored(GLFWwindow *pWindow)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowRestored();
}

void ae::WindowManager::RecordWindowMoved(GLFWwindow *pWindow, uint32_t x, uint32_t y)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowMoved(x, y);
}

void ae::WindowManager::RecordMonitor(GLFWmonitor *pMonitor, int event)
{
    for (Window *pWindow : m_Windows)
    {
        pWindow->OnMonitor(pMonitor, event);
    }
}

void ae::WindowManager::RecordFramebufferResize(GLFWwindow *pWindow, uint32_t width, uint32_t height)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnFramebufferResize(width, height);
}

void ae::WindowManager::RecordContentScaleChanged(GLFWwindow *pWindow, float xScale, float yScale)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnContentScaleChanged(xScale, yScale);
}

void ae::WindowManager::RecordFileDrop(GLFWwindow *pWindow, int count, const char **paths)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnFileDrop(count, paths);
}

void ae::WindowManager::RecordWindowClose(GLFWwindow *pWindow)
{
#ifdef AE_DEBUG
    if (!m_WindowMap.contains(pWindow))
    {
        AE_LOG(AE_WARNING, "Tried to record event but Window was not found in WindowManager");
        return;
    }
#endif // AE_DEBUG
    m_Windows[m_WindowMap[pWindow]]->OnWindowClose();
}

void ae::WindowManager::RecordControllerConnected(int controllerId)
{
    for (Window *pWindow : m_Windows)
    {
        pWindow->OnControllerConnected(controllerId);
    }
}

void ae::WindowManager::RecordControllerDisconnected(int controllerId)
{
    for (Window *pWindow : m_Windows)
    {
        pWindow->OnControllerDisconnected(controllerId);
    }
}

void ae::WindowManager::DeactivateAllWindows()
{
    for (Window *pWindow : m_Windows)
    {
        pWindow->Deactivate();
    }
}
