#include "general/pch.h"

#include "Window.h"

#include "stb_image/stb_image.h"

#include "Files.h"

ae::IconSetContainer::IconSetContainer()
	: m_Images(), m_Pixels()
{
}

ae::IconSetContainer::~IconSetContainer()
{

}

void ae::IconSetContainer::AddImage(uint8_t* pPixels, int32_t width, int32_t height)
{
	size_t offset = m_Pixels.size();
	size_t size = static_cast<size_t>(width) * height * 4;

	m_Pixels.resize(offset + size);
	memcpy(m_Pixels.data() + offset, pPixels, size);

	GLFWimage image = {};
	image.width = width;
	image.height = height;
	image.pixels = m_Pixels.data() + offset;

	m_Images.push_back(image);
}

ae::IconSet::IconSet(std::initializer_list<std::string> paths)
	: m_pContainer(nullptr)
{
	for (const std::string& path : paths)
	{
		int32_t width, height, channels;

		uint8_t* pPixels = stbi_load(AE_FILE_PATH(path).c_str(), &width, &height, &channels, 4);

		if (pPixels)
		{
			m_pContainer = std::make_shared<IconSetContainer>();
			m_pContainer->AddImage(pPixels, width, height);

			stbi_image_free(pPixels);
		}

		else
		{
			AE_THROW_FILE_NOT_FOUND_ERROR("Failed to load Icon image as the file could not be opened");
		}
	}
}

ae::IconSet::IconSet(const IconSet& icon)
	: m_pContainer(icon.m_pContainer)
{
}

ae::IconSet::~IconSet()
{
	
}

ae::IconSet& ae::IconSet::operator=(const IconSet& icon)
{
	m_pContainer = icon.m_pContainer;
	return *this;
}
