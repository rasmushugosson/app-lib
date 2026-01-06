#pragma once

#include "Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstddef>

namespace ae
{

/// Converts a byte offset to a void pointer for use with glVertexAttribPointer.
/// When using VBOs, the pointer parameter is reinterpreted as a byte offset into the buffer.
inline const void *BufferOffset(std::size_t offset)
{
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<const void *>(offset);
}

} // namespace ae

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
