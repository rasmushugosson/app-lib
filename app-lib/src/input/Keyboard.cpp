#include "general/pch.h"

#include "Window.h"

ae::Keyboard::Keyboard() = default;

ae::Keyboard::~Keyboard() = default;

bool ae::Keyboard::IsKeyPressed(int32_t key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        return false;
    }

    return m_Keys[key];
}

bool ae::Keyboard::IsKeyPressed(Key key) const
{
    return IsKeyPressed(static_cast<int32_t>(key));
}

bool ae::Keyboard::WasKeyPressed(int32_t key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        return false;
    }

    return m_Keys[key] && !m_PreviousKeys[key];
}

bool ae::Keyboard::WasKeyPressed(Key key) const
{
    return WasKeyPressed(static_cast<int32_t>(key));
}

bool ae::Keyboard::WasKeyReleased(int32_t key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        return false;
    }

    return !m_Keys[key] && m_PreviousKeys[key];
}

bool ae::Keyboard::WasKeyReleased(Key key) const
{
    return WasKeyReleased(static_cast<int32_t>(key));
}

std::string ae::Keyboard::GetTyped()
{
    std::string typed = m_Typed.str();
    m_Typed.str("");
    return typed;
}

void ae::Keyboard::SetKeyPressed(int32_t key, bool pressed)
{
    m_Keys[key] = pressed;
}

void ae::Keyboard::SetKeyTyped(int32_t key)
{
    m_Typed << static_cast<char>(key);
}

void ae::Keyboard::UpdatePreviousState()
{
    m_PreviousKeys = m_Keys;
}
