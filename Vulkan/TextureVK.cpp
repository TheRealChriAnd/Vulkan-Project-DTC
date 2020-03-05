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

void TextureVK::transitionImageLayout(uint32_t src, uint32_t dst, uint32_t layerCount)
{
	CommandBufferVK commandBuffer(m_Device, true);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout						= VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout						= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex				= src;
	barrier.dstQueueFamilyIndex				= dst;
	barrier.image							= m_Image;
	barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount		= layerCount;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	barrier.srcAccessMask	= 0;
	barrier.dstAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;

	sourceStage				= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	destinationStage		= VK_PIPELINE_STAGE_TRANSFER_BIT;

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

void TextureVK::acquireFromQueue(uint32_t src, uint32_t dst, uint32_t layerCount)
{
	CommandBufferVK commandBuffer(m_Device, true);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = src;
	barrier.dstQueueFamilyIndex = dst;
	barrier.image = m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = 0;

	sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

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

void TextureVK::releaseFromQueue(uint32_t src, uint32_t dst, uint32_t layerCount)
{
	CommandBufferVK commandBuffer(m_Device, false);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = src;
	barrier.dstQueueFamilyIndex = dst;
	barrier.image = m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

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

void TextureVK::transfer(BufferVK* stagingBuffer, int height, int width, const std::vector<VkBufferImageCopy>& region, uint32_t layerCount)
{
	QueueFamilyIndices indices = m_Device->getQueueFamilies();
	uint32_t src = indices.m_TransferFamily.value();
	uint32_t dst = indices.m_GraphicsFamily.value();

	transitionImageLayout(VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED);
	copyBufferToImage(m_Device, *stagingBuffer, static_cast<uint32_t>(width), static_cast<uint32_t>(height), region);

	acquireFromQueue(src, dst);
	releaseFromQueue(src, dst);
}
