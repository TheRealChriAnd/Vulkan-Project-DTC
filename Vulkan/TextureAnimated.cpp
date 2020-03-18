#include "TextureAnimated.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "opencv2/opencv.hpp"
#include "ThreadManager.h"
#include "VideoSource.h"

TextureAnimated::TextureAnimated(DeviceVK* device, VideoSource* source) :
	TextureVK(device),
	m_Source(nullptr),
	m_HasUpdate(false),
	m_UseFirstImage(false)
{
	m_Source = source;
	int frameSize = source->getFrameSize();
	int width = source->getWidth();
	int height = source->getHeight();

	m_StagingBuffer = new BufferVK(device, frameSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	device->createImage(width, height, 1, 0, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);
	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1);

	device->createImage(width, height, 1, 0, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image2, m_ImageMemory2);
	m_ImageView2 = m_Device->createImageView(m_Image2, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1);

	m_Region = {};
	m_Region.bufferOffset = 0;
	m_Region.bufferRowLength = 0;
	m_Region.bufferImageHeight = 0;
	m_Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	m_Region.imageSubresource.mipLevel = 0;
	m_Region.imageSubresource.baseArrayLayer = 0;
	m_Region.imageSubresource.layerCount = 1;
	m_Region.imageOffset = { 0, 0, 0 };
	m_Region.imageExtent = {
		(uint32_t)width,
		(uint32_t)height,
		1
	};

	transfer(m_Image, m_StagingBuffer, width, height, { m_Region });
	transfer(m_Image2, m_StagingBuffer, width, height, { m_Region });
	releaseFromQueue(false, m_Image);

	m_Source->addListener(this);
}

TextureAnimated::~TextureAnimated()
{
	m_Source->removeListener(this);
	delete m_StagingBuffer;
	VkDevice device = m_Device->getDevice();
	vkDestroyImage(device, m_Image2, nullptr);
	vkDestroyImageView(device, m_ImageView2, nullptr);
	vkFreeMemory(device, m_ImageMemory2, nullptr);
}

VkImageView TextureAnimated::getImageView2() const
{
	return m_ImageView2;
}

VkImage TextureAnimated::getImage2() const
{
	return m_Image2;
}

VkDeviceMemory TextureAnimated::getDeviceMemory2() const
{
	return m_ImageMemory2;
}

bool TextureAnimated::useFirstImage() const
{
	return m_UseFirstImage;
}

void TextureAnimated::update(float deltaSeconds)
{
	if (m_HasUpdate)
	{
#ifdef VULKAN
		QueueFamilyIndices indices = m_Device->getQueueFamilies();
		uint32_t src = indices.m_TransferFamily.value();
		uint32_t dst = indices.m_GraphicsFamily.value();
		std::lock_guard<SpinLock> lock(m_Lock);
		acquireFromQueue(false, m_UseFirstImage ? m_Image2 : m_Image);
		releaseFromQueue(false, m_UseFirstImage ? m_Image : m_Image2);
#else
		transfer(m_UseFirstImage ? m_Image2 : m_Image, m_StagingBuffer, getWidth(), getHeight(), { m_Region });
#endif
		m_HasUpdate = false;
		m_UseFirstImage = !m_UseFirstImage;
	}
}

int TextureAnimated::getWidth() const
{
	return m_Source->getWidth();
}

int TextureAnimated::getHeight() const
{
	return m_Source->getHeight();
}

void TextureAnimated::setOnFrameReadyCallback(const std::function<void(TextureAnimated*)>& function)
{
	m_OnFrameReadyCallback = function;
}

glm::u8vec3 TextureAnimated::getColor(int x, int y) const
{
	int index = (y * getWidth() + x) * 4;
	unsigned const char* data = m_Source->getFrameData();
	return glm::u8vec3(data[index], data[index + 1], data[index + 2]);
}

glm::vec3 TextureAnimated::getSampleColor(int width, int height, int offsetX, int offsetY) const
{
	float samples = width * height;
	glm::vec3 sampleColor(0);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			sampleColor += getColor(x + offsetX, y + offsetY);
		}
	}
	return sampleColor / samples;
}

void TextureAnimated::onFrameReady(VideoSource* source)
{
	if (m_OnFrameReadyCallback)
		m_OnFrameReadyCallback(this);

	m_StagingBuffer->writeData(source->getFrameData(), static_cast<size_t>(source->getFrameSize()));

	QueueFamilyIndices indices = m_Device->getQueueFamilies();
	uint32_t src = indices.m_TransferFamily.value();
	uint32_t dst = indices.m_GraphicsFamily.value();

	std::lock_guard<SpinLock> lock(m_Lock);
	VkImage image = m_UseFirstImage ? m_Image2 : m_Image;

#ifdef VULKAN
	releaseFromQueue(true, image);
	copyBufferToImage(m_Device, image, true, *m_StagingBuffer, static_cast<uint32_t>(source->getWidth()), static_cast<uint32_t>(source->getHeight()), { m_Region });
	acquireFromQueue(true, image);
#endif

	if (m_OnFrameReadyCallback)
		m_OnFrameReadyCallback(this);

	m_HasUpdate = true;
}
