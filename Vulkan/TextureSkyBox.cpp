#include "TextureSkyBox.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "stb_image.h"
#include <iostream>

#define IMAGE_LAYERS 6

TextureSkyBox::TextureSkyBox(DeviceVK* device) : TextureVK(device)
{
}

TextureSkyBox::~TextureSkyBox()
{
}

void TextureSkyBox::loadFromFile(const TextureLayers& layers)
{
	int texChannels;
	unsigned char* pixelData = nullptr;
	for (int i = 0; i < IMAGE_LAYERS; i++)
	{
		stbi_uc* pixels = stbi_load(layers.fileNames[i].c_str(), &m_Width, &m_Height, &texChannels, STBI_rgb_alpha);
		if (i == 0)
		{
			m_SizePerImage = m_Width * m_Height * 4;
			m_Size = m_SizePerImage * 6;
			pixelData = new stbi_uc[m_Size];
		}
		 memcpy(pixelData + i * m_SizePerImage, pixels, m_SizePerImage);
		 stbi_image_free(pixels);
	}

	m_Device->createImage(m_Width, m_Height, IMAGE_LAYERS, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);
	BufferVK stagingBuffer(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.writeData(pixelData, static_cast<size_t>(m_Size));
	delete[] pixelData;

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	for (int i = 0; i < IMAGE_LAYERS; i++)
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = m_SizePerImage * i;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = i;
		region.imageSubresource.layerCount = 1;
		region.imageExtent = { (uint32_t)m_Width, (uint32_t)m_Height, (uint32_t)1 };
		bufferCopyRegions.push_back(region);
	}

	transfer(m_Image, &stagingBuffer, m_Width, m_Height, bufferCopyRegions, IMAGE_LAYERS);
	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, IMAGE_LAYERS);
}
