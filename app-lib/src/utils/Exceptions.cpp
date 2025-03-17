#include "general/pch.h"

#include "OpenGL.h"
#include "Vulkan.h"
#include "OpenAL.h"

#ifndef AE_DIST

static constexpr std::array<std::string_view, 8> s_OpenGLErrorLookup = {
	"GL_NO_ERROR",
	"GL_INVALID_ENUM",
	"GL_INVALID_VALUE",
	"GL_INVALID_OPERATION",
	"GL_STACK_OVERFLOW",
	"GL_STACK_UNDERFLOW",
	"GL_OUT_OF_MEMORY",
	"GL_INVALID_FRAMEBUFFER_OPERATION"
};

void ae::OpenGLClearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

void ae::OpenGLCheckErrors(const std::string& function, const std::string& file, int line)
{
	GLenum error;

	do
	{
		error = glGetError();

		if (error != GL_NO_ERROR)
		{
			std::string fileName = "Unknown file";

			size_t lastSlash = file.find_last_of("/\\");
			if (lastSlash != std::string::npos)
			{
				fileName = file.substr(lastSlash + 1);
			}

			if (error >= 0 && error < 8)
			{
				AE_THROW_OPENGL_ERROR("Error code: " << s_OpenGLErrorLookup[error] << " from call " << function << " in file " << fileName << " at line " << line);
			}
			else
			{
				AE_THROW_OPENGL_ERROR("Error code: " << error << " from call " << function << " in file " << fileName << " at line " << line);
			}
		}
	} while (error != GL_NO_ERROR);
}

void ae::VulkanCheckResult(VkResult result, const char* function, const char* file, uint32_t line)
{
	if (result != VK_SUCCESS)
	{
		AE_LOG_CONSOLE(AE_ERROR, "Result: " << result << " in function " << function << " in file " << file << " at line " << line);
	}
}

static constexpr std::array<std::string_view, 6> s_OpenALLookup = {
	"AL_NO_ERROR", 
	"AL_INVALID_NAME", 
	"AL_INVALID_ENUM", 
	"AL_INVALID_VALUE", 
	"AL_INVALID_OPERATION", 
	"AL_OUT_OF_MEMORY" 
};

void ae::OpenALClearErrors()
{
	while (alGetError() != AL_NO_ERROR);
}

void ae::OpenALCheckErrors(const std::string& function, const std::string& file, int line)
{
	ALenum error;

	do
	{
		error = alGetError();

		if (error != AL_NO_ERROR)
		{
			std::string fileName = "Unknown file";

			size_t lastSlash = file.find_last_of("/\\");
			if (lastSlash != std::string::npos)
			{
				fileName = file.substr(lastSlash + 1);
			}

			if (error >= 0 && error < 6)
			{
				AE_THROW_OPENAL_ERROR("Error code: " << s_OpenALLookup[error] << " from call " << function << " in file " << fileName << " at line " << line);
			}
			else
			{
				AE_THROW_OPENAL_ERROR("Error code: " << error << " from call " << function << " in file " << fileName << " at line " << line);
			}
		}
	} while (error != AL_NO_ERROR);
}

#else

#define GL_CHECK(x) x

#endif // AE_DIST
