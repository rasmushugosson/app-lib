#pragma once

#include "Log.h"
#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace ae
{
class WindowManager
{
  public:
    WindowManager();
    WindowManager(const WindowManager &) = delete;
    WindowManager &operator=(const WindowManager &) = delete;
    ~WindowManager();

    inline static WindowManager &Get()
    {
        static WindowManager instance;
        return instance;
    }

    void AddWindow(Window *window);
    void RemoveWindow(Window *window);

    void RecordKey(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void RecordChar(GLFWwindow *pWindow, unsigned int c);

    void RecordMouseButton(GLFWwindow *pWindow, int button, int action, int mods);
    void RecordMouseMoved(GLFWwindow *pWindow, double x, double y);
    void RecordMouseScrolled(GLFWwindow *pWindow, double x, double y);
    void RecordMouseEntered(GLFWwindow *pWindow, int entered);

    void RecordWindowResize(GLFWwindow *pWindow, uint32_t width, uint32_t height);
    void RecordWindowMinimalized(GLFWwindow *pWindow);
    void RecordWindowMaximalized(GLFWwindow *pWindow);
    void RecordWindowRestored(GLFWwindow *pWindow);
    void RecordWindowMoved(GLFWwindow *pWindow, uint32_t x, uint32_t y);
    void RecordWindowFocused(GLFWwindow *pWindow, int focused);

    void RecordMonitor(GLFWmonitor *pMonitor, int event);

    void RecordFramebufferResize(GLFWwindow *pWindow, uint32_t width, uint32_t height);
    void RecordContentScaleChanged(GLFWwindow *pWindow, float xScale, float yScale);
    void RecordFileDrop(GLFWwindow *pWindow, int count, const char** paths);
    void RecordWindowClose(GLFWwindow *pWindow);
    void RecordControllerConnected(int controllerId);
    void RecordControllerDisconnected(int controllerId);

    void DeactivateAllWindows();

    void EnsureInitialized();

  private:
    void Init();
    void Terminate();

  private:
    bool m_Initialized;
    std::vector<Window *> m_Windows;
    std::unordered_map<GLFWwindow *, uint32_t> m_WindowMap;
};
} // namespace ae
