#pragma once

#include "Log.h"

#include <AL/al.h>
#include <AL/alc.h>

#ifndef AE_DIST

namespace ae
{
void OpenALClearErrors();
void OpenALCheckErrors(const std::string &function, const std::string &file, int line);

#define AL_CHECK(x)                                                                                                    \
    ae::OpenALClearErrors();                                                                                           \
    x;                                                                                                                 \
    ae::OpenALCheckErrors(#x, __FILE__, __LINE__)
} // namespace ae

#else

#define AL_CHECK(x) x

#endif // AE_DIST
