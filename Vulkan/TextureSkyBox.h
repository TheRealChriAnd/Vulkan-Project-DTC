#pragma once
#include "Defines.h"
#include "TextureVK.h"
#include <string>
#include <vector>

class BufferVK;

struct TextureLayers
{
	std::string fileNames[6];
};

class TextureSkyBox : public TextureVK
{
public:
	TextureSkyBox(DeviceVK* device);
	~TextureSkyBox();

	void loadFromFile(const TextureLayers& layers);

private:
	VkDeviceSize m_Size;
	VkDeviceSize m_SizePerImage;
	int m_Width;
	int m_Height;
};
