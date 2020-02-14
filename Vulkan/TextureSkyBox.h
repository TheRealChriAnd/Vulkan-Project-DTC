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
	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height);

private:
	VkDeviceSize m_Size;
	VkDeviceSize m_SizePerImage;
	int m_Width;
	int m_Height;

	bool m_FirstImage;
	std::vector<VkBufferImageCopy> bufferCopyRegions;
	unsigned char* m_PixelData;
};
