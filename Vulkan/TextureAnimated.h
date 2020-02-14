#pragma once
#include "TextureVK.h"
#include <string>
#include <atomic>

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

	void submit();

private:
	void run();
	void update(float deltaSeconds);

private:
	BufferVK* m_StagingBuffer;
	cv::VideoCapture* m_VideoCapture;
	VkBufferImageCopy m_Region;
	double m_SleepTime;
	double m_Timer;
	int m_TotalSize;
	int m_Width;
	int m_Height;
	unsigned char* m_PixelData;
	std::atomic_bool m_HasUpdate;
	std::atomic_bool m_Shutdown;
	std::thread* m_Thread;
};
