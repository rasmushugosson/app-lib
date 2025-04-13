#include "general/pch.h"

#include "Window.h"

#include "stb_image.h"

#include "Files.h"

ae::CursorContainer::CursorContainer(GLFWcursor* pCursor)
	: m_pCursor(pCursor)
{
}

ae::CursorContainer::~CursorContainer()
{
	if (m_pCursor)
	{
		glfwDestroyCursor(m_pCursor);
	}

	AE_LOG_CONSOLE(AE_TRACE, "Cursor destroyed");
}

ae::Cursor::Cursor(const std::string& path)
	: m_pContainer(nullptr), m_XHot(0), m_YHot(0)
{
	int32_t width, height, channels;
	uint8_t* pPixels = stbi_load(AE_FILE_PATH(path).c_str(), &width, &height, &channels, 4);

	if (pPixels)
	{
		GLFWimage image = {};

		image.width = width;
		image.height = height;
		image.pixels = pPixels;

		GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);

		if (cursor)
		{
			m_pContainer = std::make_shared<CursorContainer>(cursor);
			AE_LOG_CONSOLE(AE_TRACE, "Cursor created from path: " << path);
		}

		else
		{
			AE_LOG_CONSOLE(AE_WARNING, "Failed to create Cursor from image path '" << path << "'");
			m_pContainer = std::make_shared<CursorContainer>(nullptr);
		}
	}

	else
	{
		AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load Cursor image as the file could not be opened");
	}

	stbi_image_free(pPixels);
}

ae::Cursor::Cursor(const std::string& path, int32_t xHot, int32_t yHot)
	: m_pContainer(nullptr), m_XHot(xHot), m_YHot(yHot)
{
	int32_t width, height, channels;
	uint8_t* pPixels = stbi_load(AE_FILE_PATH(path).c_str(), &width, &height, &channels, 4);

	if (pPixels)
	{
		GLFWimage image = {};
		image.width = width;
		image.height = height;
		image.pixels = pPixels;

		GLFWcursor* cursor = glfwCreateCursor(&image, xHot, yHot);

		if (cursor)
		{
			m_pContainer = std::make_shared<CursorContainer>(cursor);
			AE_LOG_CONSOLE(AE_TRACE, "Cursor created from path: " << path);
		}

		else
		{
			AE_LOG_CONSOLE(AE_WARNING, "Failed to create Cursor from image path '" << path << "'");
			m_pContainer = std::make_shared<CursorContainer>(nullptr);
		}
	}

	else
	{
		AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load Cursor image as the file could not be opened");
	}

	stbi_image_free(pPixels);
}

ae::Cursor::Cursor(const Cursor& cursor)
	: m_pContainer(cursor.m_pContainer), m_XHot(cursor.m_XHot), m_YHot(cursor.m_YHot)
{
}

ae::Cursor::~Cursor()
{
}

ae::Cursor& ae::Cursor::operator=(const Cursor& cursor)
{
	m_pContainer = cursor.m_pContainer;
	return *this;
}
