#include "TextureAnimated.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "opencv2/opencv.hpp"
#include <thread>

TextureAnimated::TextureAnimated(DeviceVK* device, const std::string& file) : TextureVK(device), m_Timer(0), m_HasUpdate(false)
{
	m_VideoCapture = new cv::VideoCapture(file);
	double fps = m_VideoCapture->get(cv::CAP_PROP_FPS);
	m_SleepTime = 1.0F / fps;

	if (!m_VideoCapture->isOpened())
		std::cout << "Error opening video stream or file" << std::endl;

	m_Width = m_VideoCapture->get(cv::CAP_PROP_FRAME_WIDTH);
	m_Height = m_VideoCapture->get(cv::CAP_PROP_FRAME_HEIGHT);
	m_TotalSize = m_Width * m_Height * 4;

	m_StagingBuffer = new BufferVK(device, m_TotalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	device->createImage(m_Width, m_Height, 1, 0, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1);

	m_PixelData = new unsigned char[m_TotalSize];
	memset(m_PixelData, 255, m_TotalSize);

	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(m_Device, *m_StagingBuffer, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_Thread = new std::thread(&TextureAnimated::run, this);
}

TextureAnimated::~TextureAnimated()
{
	m_VideoCapture->release();
	delete m_VideoCapture;
	delete[] m_PixelData;
	delete m_StagingBuffer;
	delete m_Thread;
}

void TextureAnimated::update(float deltaSeconds)
{
	m_Timer += deltaSeconds;
	if (m_Timer < m_SleepTime)
		return;

	m_Timer -= m_SleepTime;

	cv::Mat frame;
	(*m_VideoCapture) >> frame;

	if (frame.empty())
		return;

	static int count = m_Width * m_Height * 3;
	int offset = 0;
	for (int i = 0; i < count; i += 3)
	{
		m_PixelData[i + offset] = frame.data[i + 2];
		m_PixelData[i + offset + 1] = frame.data[i + 1];
		m_PixelData[i + offset + 2] = frame.data[i];
		offset += 1;
	}
	m_HasUpdate = true;
}

void TextureAnimated::update()
{
	if (m_HasUpdate)
	{
		m_StagingBuffer->writeData(m_PixelData, static_cast<size_t>(m_TotalSize));
		m_HasUpdate = false;

		transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(m_Device, *m_StagingBuffer, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
		transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

void TextureAnimated::transitionImageLayout(DeviceVK * device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
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
	barrier.subresourceRange.layerCount = 1;

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

void TextureAnimated::copyBufferToImage(DeviceVK * device, const BufferVK & buffer, uint32_t width, uint32_t height)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

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
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer.getCommandBuffer(), buffer.getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.end();
	commandBuffer.submit();
}

void TextureAnimated::run()
{
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (true)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		update(delta);
	}
}
