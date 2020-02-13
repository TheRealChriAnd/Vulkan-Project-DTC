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
	for (int i = 0; i < IMAGE_LAYERS; i++)
	{
		stbi_uc* pixels = stbi_load(layers.fileNames[i].c_str(), &m_Width, &m_Height, &texChannels, STBI_rgb_alpha);
		if (i == 0)
		{
			m_SizePerImage = m_Width * m_Height * 4;
			m_Size = m_SizePerImage * 6;
			m_PixelData = new stbi_uc[m_Size];
		}
		 memcpy(m_PixelData + i * m_SizePerImage, pixels, m_SizePerImage);
		 stbi_image_free(pixels);
	}

	m_Device->createImage(m_Width, m_Height, IMAGE_LAYERS, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);
	BufferVK buffer(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	buffer.writeData(m_PixelData, static_cast<size_t>(m_Size));
	delete[] m_PixelData;

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

	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(m_Device, buffer, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, IMAGE_LAYERS);
}

void TextureSkyBox::transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = IMAGE_LAYERS;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer.getCommandBuffer(),
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	commandBuffer.end();
	commandBuffer.submit();
}

void TextureSkyBox::copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

	vkCmdCopyBufferToImage(commandBuffer.getCommandBuffer(), buffer.getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

	commandBuffer.end();
	commandBuffer.submit();
}
