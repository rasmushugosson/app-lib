#pragma once

#include "DearImGui.h"

#include "Interface.h"

namespace ae
{
	class OpenGLInterface : public Interface
	{
	public:
		OpenGLInterface(Window& window);
		OpenGLInterface(const OpenGLInterface&) = delete;
		OpenGLInterface& operator=(const OpenGLInterface&) = delete;
		virtual ~OpenGLInterface();
	protected:
		virtual bool CreateImpl() override;
		virtual void DestroyImpl() override;

		virtual void PrepareImpl() override;
		virtual void FinishImpl() override;
	};
}
