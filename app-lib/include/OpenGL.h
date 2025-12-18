#pragma once

#include "Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef AE_DIST

namespace ae
{
void OpenGLClearErrors();
void OpenGLCheckErrors(const std::string &function, const std::string &file, int line);

#define GL_CHECK(x)                                                                                                    \
    ae::OpenGLClearErrors();                                                                                           \
    x;                                                                                                                 \
    ae::OpenGLCheckErrors(#x, __FILE__, __LINE__)
} // namespace ae

#else

#define GL_CHECK(x) x

#endif // AE_DIST
