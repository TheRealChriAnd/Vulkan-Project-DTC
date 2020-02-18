#include "TextureAnimated.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "CommandBufferVK.h"
#include "opencv2/opencv.hpp"
#include "ThreadManager.h"

TextureAnimated::TextureAnimated(DeviceVK* device, const std::string& file) :
	TextureVK(device),
	m_Timer(0),
	m_HasUpdate(false),
	m_Playing(false),
	m_FrameCount(0),
	m_CurrentFrame(0)
{
	m_VideoCapture = new cv::VideoCapture(file);
	double fps = m_VideoCapture->get(cv::CAP_PROP_FPS);
	m_FrameCount = m_VideoCapture->get(cv::CAP_PROP_FRAME_COUNT);
	m_SleepTime = 1.0F / fps;

	if (!m_VideoCapture->isOpened())
		throw std::runtime_error("Error opening video stream or file!");

	m_Width = m_VideoCapture->get(cv::CAP_PROP_FRAME_WIDTH);
	m_Height = m_VideoCapture->get(cv::CAP_PROP_FRAME_HEIGHT);
	m_TotalSize = m_Width * m_Height * 4;

	m_StagingBuffer = new BufferVK(device, m_TotalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	device->createImage(m_Width, m_Height, 1, 0, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1);

	m_PixelData = new unsigned char[m_TotalSize];
	memset(m_PixelData, 255, m_TotalSize);


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
		(uint32_t)m_Width,
		(uint32_t)m_Height,
		1
	};

	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(m_Device, *m_StagingBuffer, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), { m_Region });
	transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

TextureAnimated::~TextureAnimated()
{
	this->stop();
	m_VideoCapture->release();
	delete m_VideoCapture;
	delete[] m_PixelData;
	delete m_StagingBuffer;
}

void TextureAnimated::play()
{
	m_Playing = true;
	ThreadManager::addAsynchronousService(this);
}

void TextureAnimated::stop()
{
	m_Playing = false;
	ThreadManager::removeAsynchronousService(this);
}

void TextureAnimated::submit()
{
	if (m_HasUpdate)
	{
		m_StagingBuffer->writeData(m_PixelData, static_cast<size_t>(m_TotalSize));
		m_HasUpdate = false;

		transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(m_Device, *m_StagingBuffer, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), { m_Region });
		transitionImageLayout(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

bool TextureAnimated::isPlaying() const
{
	return m_Playing;
}

const glm::mat4& TextureAnimated::getSampledCornerColors() const
{
	glm::mat4 averageColors;
	int temp;
	averageColors[0] = glm::vec4(m_PixelData[0] * 0.00392f, m_PixelData[1] * 0.00392f, m_PixelData[2] * 0.00392f, 1.0f);
	temp = m_Width * 3;
	averageColors[1] = glm::vec4(m_PixelData[temp] * 0.00392f, m_PixelData[temp + 1] * 0.00392f, m_PixelData[temp + 2] * 0.00392f, 1.0);
	temp = m_Height * m_Width * 3 - m_Width;
	averageColors[2] = glm::vec4(m_PixelData[temp] * 0.00392f, m_PixelData[temp + 1] * 0.00392f, m_PixelData[temp + 2] * 0.00392f, 1.0);
	temp = m_Height * m_Width * 3;
	averageColors[3] = glm::vec4(m_PixelData[temp] * 0.00392f, m_PixelData[temp + 1] * 0.00392f, m_PixelData[temp + 2] * 0.00392f, 255);
	
	return averageColors;
}

double timer = 0;
int ups = 0;

void TextureAnimated::updateAsynchronous(float deltaSeconds)
{
	update(deltaSeconds);

	ups++;
	timer += deltaSeconds;
	if (timer >= 1.0F)
	{
		timer -= 1.0F;
		//std::cout << "UPS: " << ups << std::endl;
		ups = 0;
	}
}

void TextureAnimated::update(float deltaSeconds)
{
	m_Timer += deltaSeconds;
	if (m_Timer < m_SleepTime)
		return;

	m_Timer -= m_SleepTime;
	m_CurrentFrame++;

	if (m_CurrentFrame == m_FrameCount)
		this->stop();

	if (deltaSeconds > m_SleepTime)
	{
		m_VideoCapture->grab();
		return;
	}
		
	static cv::Mat frame;
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