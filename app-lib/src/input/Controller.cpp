#include "general/pch.h"

#include "Window.h"

ae::Controller::Controller(uint32_t id) : m_Id(id) {}

bool ae::Controller::IsButtonPressed(int32_t button) const
{
    if (button < 0 || button > AE_CONTROLLER_BUTTON_LAST)
    {
        return false;
    }

    GLFWgamepadstate state = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + static_cast<int>(m_Id), &state))
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get button state but controller {} is not connected", m_Id);
#endif
        return false;
    }

    return state.buttons[button] == GLFW_PRESS;
}

bool ae::Controller::IsButtonPressed(ControllerButton button) const
{
    return IsButtonPressed(static_cast<int32_t>(button));
}

float ae::Controller::GetAxis(int32_t axis) const
{
    if (axis < 0 || axis > AE_CONTROLLER_AXIS_LAST)
    {
        return 0.0f;
    }

    GLFWgamepadstate state = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + static_cast<int>(m_Id), &state))
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get axis state but controller {} is not connected", m_Id);
#endif
        return 0.0f;
    }

    return ApplyDeadzone(state.axes[axis]);
}

float ae::Controller::GetAxis(ControllerAxis axis) const
{
    return GetAxis(static_cast<int32_t>(axis));
}

ae::Vec2 ae::Controller::GetLeftStick() const
{
    GLFWgamepadstate state = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + static_cast<int>(m_Id), &state))
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get left stick but controller {} is not connected", m_Id);
#endif
        return {0.0f, 0.0f};
    }

    return {ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]),
            ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y])};
}

ae::Vec2 ae::Controller::GetRightStick() const
{
    GLFWgamepadstate state = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + static_cast<int>(m_Id), &state))
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get right stick but controller {} is not connected", m_Id);
#endif
        return {0.0f, 0.0f};
    }

    return {ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]),
            ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y])};
}

ae::Vec2 ae::Controller::GetTriggers() const
{
    GLFWgamepadstate state = {};
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + static_cast<int>(m_Id), &state))
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get triggers but controller {} is not connected", m_Id);
#endif
        return {0.0f, 0.0f};
    }

    // Triggers don't need deadzone - they're linear from -1 to 1
    return {state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER],
            state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]};
}

float ae::Controller::ApplyDeadzone(float value) const
{
    if (std::abs(value) < m_Deadzone)
    {
        return 0.0f;
    }

    // Scale the value so it starts from 0 after the deadzone
    float sign = (value > 0.0f) ? 1.0f : -1.0f;
    return sign * (std::abs(value) - m_Deadzone) / (1.0f - m_Deadzone);
}

std::string ae::Controller::GetName() const
{
    const char *pName = glfwGetGamepadName(GLFW_JOYSTICK_1 + static_cast<int>(m_Id));
    if (pName == nullptr)
    {
#ifdef AE_DEBUG
        AE_LOG(AE_WARNING, "Tried to get name but controller {} is not connected", m_Id);
#endif
        return "";
    }
    return pName;
}

bool ae::Controller::IsConnected(uint32_t id)
{
    return glfwJoystickPresent(GLFW_JOYSTICK_1 + static_cast<int>(id)) &&
           glfwJoystickIsGamepad(GLFW_JOYSTICK_1 + static_cast<int>(id));
}
