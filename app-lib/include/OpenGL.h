#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Log.h"

#ifndef AE_DIST

namespace ae
{
	void OpenGLClearErrors();
	void OpenGLCheckErrors(const std::string& function, const std::string& file, int line);

#define GL_CHECK(x) ae::OpenGLClearErrors(); x; ae::OpenGLCheckErrors(#x, __FILE__, __LINE__)

	class OpenGLError : public std::runtime_error
	{
	public:
		explicit OpenGLError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("OpenGL error", file, line, message))
		{
		}
	};

#define AE_THROW_OPENGL_ERROR(m) throw ae::OpenGLError(__FILE__, __LINE__, std::ostringstream() << m)
}

#else

#define GL_CHECK(x) x

#endif // AE_DIST
