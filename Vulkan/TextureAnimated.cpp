#include "TextureAnimated.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "opencv2/opencv.hpp"
#include "ThreadManager.h"
#include "VideoSource.h"
#include "Profiler.h"

std::atomic_int TextureAnimated::m_LostFrames = 0;
int TextureAnimated::m_Counter = 0;

TextureAnimated::TextureAnimated(DeviceVK* device, VideoSource* source) :
	TextureVK(device),
	m_Source(nullptr),
	m_HasUpdate(false),
	m_UseFirstImage(false),
	m_HasUnWritenData(false)
{
	m_Counter = 0;
	m_LostFrames = 0;

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

	m_Id = m_Counter++ % 3;

#ifdef VULKAN 
	ThreadManager::addAsynchronousService(this, 1);
#endif
}

TextureAnimated::~TextureAnimated()
{
#ifdef VULKAN
	ThreadManager::removeAsynchronousService(this, 1);
#endif

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
		VkImage image = m_UseFirstImage ? m_Image2 : m_Image;
		acquireFromQueue(false, image);
		releaseFromQueue(false, m_UseFirstImage ? m_Image : m_Image2);
#else
		m_StagingBuffer->writeData(m_Source->getFrameData(), static_cast<size_t>(m_Source->getFrameSize()));
		transfer(m_UseFirstImage ? m_Image2 : m_Image, m_StagingBuffer, getWidth(), getHeight(), { m_Region });
#endif
		Profiler::count("TV_FRAMES_TRANSFERRED");
		m_HasUpdate = false;
		m_UseFirstImage = !m_UseFirstImage;
		m_LostFrames--;
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

glm::vec3 TextureAnimated::getSampleColor(int width, int height, int offsetX, int offsetY, int sampleEvery) const
{
	float samples = (int)(width / sampleEvery) * (int)(height / sampleEvery);
	glm::vec3 sampleColor(0);
	for (int x = 0; x < width; x += sampleEvery)
	{
		for (int y = 0; y < height; y += sampleEvery)
		{
			sampleColor += getColor(x + offsetX, y + offsetY);
		}
	}
	return sampleColor / samples;
}

void TextureAnimated::onFrameReady(VideoSource* source)
{
	m_LostFrames++;

	if (!source)
		return;

	if (m_OnFrameReadyCallback)
		ThreadManager::scheduleExecution(std::bind(m_OnFrameReadyCallback, this));

#ifdef VULKAN
	if (m_Id <= 1)
	{
		m_StagingBuffer->writeData(m_Source->getFrameData(), static_cast<size_t>(m_Source->getFrameSize()));
	}
	m_HasUnWritenData = true;
#else
	m_HasUpdate = true;
#endif
}

void TextureAnimated::updateAsynchronous(float deltaSeconds)
{
	if (m_HasUnWritenData)
	{
		m_HasUnWritenData = false;
		QueueFamilyIndices indices = m_Device->getQueueFamilies();
		uint32_t src = indices.m_TransferFamily.value();
		uint32_t dst = indices.m_GraphicsFamily.value();

		std::lock_guard<SpinLock> lock(m_Lock);
		VkImage image = m_UseFirstImage ? m_Image2 : m_Image;

		if (m_Id >= 2)
		{
			m_StagingBuffer->writeData(m_Source->getFrameData(), static_cast<size_t>(m_Source->getFrameSize()));
		}

		releaseFromQueue(true, image);
		copyBufferToImage(m_Device, image, true, *m_StagingBuffer, static_cast<uint32_t>(getWidth()), static_cast<uint32_t>(getHeight()), { m_Region });
		acquireFromQueue(true, image);

		m_HasUpdate = true;
	}
}

void TextureAnimated::resetLostFrames()
{
	m_LostFrames = 0;
}

int TextureAnimated::getLostFrames()
{
	return m_LostFrames;
}
