#include "TextureVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"

TextureVK::TextureVK(DeviceVK* device)
{
	m_Device = device;
}

TextureVK::~TextureVK()
{
	VkDevice device = m_Device->getDevice();

	vkDestroyImage(device, m_Image, nullptr);
	vkDestroyImageView(device, m_ImageView, nullptr);
	vkFreeMemory(device, m_ImageMemory, nullptr);
}

VkImageView TextureVK::getImageView() const
{
	return m_ImageView;
}

VkImage TextureVK::getImage() const
{
	return m_Image;
}

VkDeviceMemory TextureVK::getDeviceMemory() const
{
	return m_ImageMemory;
}

void TextureVK::transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t src, uint32_t dst, bool transfer, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags accessMaskSrc, VkAccessFlags accessMaskDst, uint32_t layerCount)
{
	CommandBufferVK commandBuffer(device, transfer);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;


	barrier.srcQueueFamilyIndex = src;
	barrier.dstQueueFamilyIndex = dst;


	barrier.image = m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

	barrier.srcAccessMask = accessMaskSrc;
	barrier.dstAccessMask = accessMaskDst;

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

void TextureVK::copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, const std::vector<VkBufferImageCopy>& regions)
{
	CommandBufferVK commandBuffer(device, true);
	commandBuffer.begin();

	vkCmdCopyBufferToImage(commandBuffer.getCommandBuffer(), buffer.getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());

	commandBuffer.end();
	commandBuffer.submit();
}