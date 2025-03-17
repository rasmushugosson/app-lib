#include "general/pch.h"

#include "Window.h"

ae::Keyboard::Keyboard()
	: m_Keys(), m_Typed()
{
}

ae::Keyboard::~Keyboard()
{
}

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
