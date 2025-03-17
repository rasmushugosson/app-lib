#include "general/pch.h"

#include "Window.h"

ae::Controller::Controller(uint32_t id)
	: m_Id(id)
{
}

ae::Controller::~Controller()
{
}

bool ae::Controller::IsButtonPressed(int32_t button) const
{
	if (button < 0 || button > AE_CONTROLLER_BUTTON_LAST)
	{
		return false;
	}
	
	const unsigned char* pButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + m_Id, nullptr);

	return pButtons[button];
}

bool ae::Controller::IsButtonPressed(ControllerButton button) const
{
	const unsigned char* pButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + m_Id, nullptr);

	return pButtons[static_cast<int32_t>(button)];
}

float ae::Controller::GetAxis(int32_t axis) const
{
	if (axis < 0 || axis > AE_CONTROLLER_AXIS_LAST)
	{
		return 0.0f;
	}

	const float* pAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + m_Id, nullptr);

	return pAxes[axis];
}

float ae::Controller::GetAxis(ControllerAxis axis) const
{
	const float* pAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + m_Id, nullptr);

	return pAxes[static_cast<int32_t>(axis)];
}

std::string ae::Controller::GetName() const
{
	std::string name = glfwGetJoystickName(GLFW_JOYSTICK_1 + m_Id);

	return name;
}

bool ae::Controller::IsConnected(uint32_t id)
{
	return glfwJoystickPresent(GLFW_JOYSTICK_1 + id) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1 + id);
}
