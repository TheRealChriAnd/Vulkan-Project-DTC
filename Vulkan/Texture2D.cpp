#include "Texture2D.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "stb_image.h"
#include <iostream>

Texture2D::Texture2D(DeviceVK* device, const std::string& file) : TextureVK(device)
{
	createTextureImage(m_Device, file);
	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1);
}

Texture2D::~Texture2D()
{
}

void Texture2D::createTextureImage(DeviceVK * device, const std::string & file)
{
	int texWidth;
	int texHeight;
	int texChannels;
	stbi_uc* pixels = stbi_load(file.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("Error: Failed to load texture image!");

	BufferVK stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.writeData(pixels, static_cast<size_t>(imageSize));

	stbi_image_free(pixels);

	device->createImage(texWidth, texHeight, 1, 0, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		(uint32_t)texWidth,
		(uint32_t)texHeight,
		1
	};

	QueueFamilyIndices indices = device->getQueueFamilies();
	uint32_t src = indices.m_TransferFamily.value();
	uint32_t dst = indices.m_GraphicsFamily.value();

	transitionImageLayout(device, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, true);
	copyBufferToImage(device, stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), { region });

	transitionImageLayout(device, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, src, dst, true);
	transitionImageLayout(device, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, src, dst, false);
}