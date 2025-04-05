#include "general/pch.h"

#include "Window.h"

ae::Context::Context(Window& window)
	: m_Window(window), m_GraphicsAPI("Undefined"), m_GraphicsVersion("Undefined"), m_GraphicsCard("Undefined"), 
	m_GraphicsVendor("Undefined"), m_Created(false)
{
}

ae::Context::~Context()
{
#ifdef AE_DEBUG
	if (m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Context not destroyed before decontruction");
	}
#endif // AE_DEBUG
}

void ae::Context::Create()
{
#ifdef AE_DEBUG
	if (m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to create graphics Context for Window but it has already been created");
		return;
	}
#endif // AE_DEBUG

	if (!CreateImpl())
	{
		AE_THROW_RUNTIME_ERROR("Failed to create graphics Context for Window");
	}

	m_Created = true;
}

void ae::Context::Activate()
{
#ifdef AE_DEBUG
	if (!m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to activate graphics Context but it is not created");
		return;
	}
#endif // AE_DEBUG
	ActivateImpl();
}

void ae::Context::Deactivate()
{
#ifdef AE_DEBUG
	if (!m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to deactivate graphics Context but it is not created");
		return;
	}
#endif // AE_DEBUG
	DeactivateImpl();
}

void ae::Context::Destroy()
{
#ifdef AE_DEBUG
	if (!m_Created)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to destroy graphics Context but it is already not created");
		return;
	}
#endif // AE_DEBUG
	DestroyImpl();
	m_Created = false;
}
