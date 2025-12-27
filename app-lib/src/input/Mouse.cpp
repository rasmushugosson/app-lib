#include "general/pch.h"

#include "Window.h"

ae::Mouse::Mouse(Window *pWindow)
    : m_pWindow(pWindow), m_DeltaX(0.0f), m_DeltaY(0.0f), m_ScrollX(0.0f), m_ScrollY(0.0f), m_Entered(false)
{
}

bool ae::Mouse::IsButtonPressed(int32_t button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }

    return m_Buttons[button];
}

bool ae::Mouse::IsButtonPressed(MouseButton button) const
{
    return m_Buttons[static_cast<int32_t>(button)];
}

float ae::Mouse::GetX() const
{
    double x;
    double y;
    glfwGetCursorPos(m_pWindow->GetWindow(), &x, &y);

    return static_cast<float>(x);
}

float ae::Mouse::GetY() const
{
    double x;
    double y;
    glfwGetCursorPos(m_pWindow->GetWindow(), &x, &y);

    return static_cast<float>(y);
}

void ae::Mouse::SetX(float x)
{
    glfwSetCursorPos(m_pWindow->GetWindow(), static_cast<double>(x), static_cast<double>(GetY()));
}

void ae::Mouse::SetY(float y)
{
    glfwSetCursorPos(m_pWindow->GetWindow(), static_cast<double>(GetX()), static_cast<double>(y));
}

void ae::Mouse::SetPosition(float x, float y)
{
    glfwSetCursorPos(m_pWindow->GetWindow(), static_cast<double>(x), static_cast<double>(y));
}

void ae::Mouse::SetPressed(int32_t button, bool pressed)
{
    m_Buttons[button] = pressed;
}

void ae::Mouse::SetMoved(float x, float y)
{
    m_DeltaX = x + m_DeltaX;
    m_DeltaY = y + m_DeltaY;
}

void ae::Mouse::SetScrolled(float x, float y)
{
    m_ScrollX = x + m_ScrollX;
    m_ScrollY = y + m_ScrollY;
}

void ae::Mouse::SetEntered(bool entered)
{
    m_Entered = entered;
}
