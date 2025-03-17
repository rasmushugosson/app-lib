#pragma once

#include "ImGui.h"

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

		virtual void PrepareFrame() override;
		virtual void FinishFrame() override;
	};
}
