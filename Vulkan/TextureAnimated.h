#pragma once
#include "TextureVK.h"
#include <string>

class BufferVK;

namespace cv
{
	class VideoCapture;
}

namespace std
{
	class thread;
}

class TextureAnimated : public TextureVK
{
public:
	TextureAnimated(DeviceVK* device, const std::string& file);
	virtual ~TextureAnimated();

	void update(float deltaSeconds);
	void update();

	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height);

	void run();

private:
	BufferVK* m_StagingBuffer;
	cv::VideoCapture* m_VideoCapture;
	double m_SleepTime;
	double m_Timer;
	int m_TotalSize;
	int m_Width;
	int m_Height;
	unsigned char* m_PixelData;
	bool m_HasUpdate;
	std::thread* m_Thread;
};
