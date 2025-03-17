#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include "Log.h"

#ifndef AE_DIST

namespace ae
{
	void OpenALClearErrors();
	void OpenALCheckErrors(const std::string& function, const std::string& file, int line);

#define AL_CHECK(x) ae::OpenALClearErrors(); x; ae::OpenALCheckErrors(#x, __FILE__, __LINE__)

	class OpenALError : public std::runtime_error
	{
	public:
		explicit OpenALError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("OpenAL error", file, line, message))
		{
		}
	};

#define AE_THROW_OPENAL_ERROR(m) throw ae::OpenALError(__FILE__, __LINE__, std::ostringstream() << m)

}

#else

#define AL_CHECK(x) x

#endif // AE_DIST
