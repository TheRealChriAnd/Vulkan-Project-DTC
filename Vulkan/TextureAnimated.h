#pragma once
#include "TextureVK.h"
#include <string>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "IAsynchronous.h"


class BufferVK;

namespace cv
{
	class VideoCapture;
}

class TextureAnimated : public TextureVK, public IAsynchronous
{
public:
	TextureAnimated(DeviceVK* device, const std::string& file);
	virtual ~TextureAnimated();

	void play();
	void stop();
	void submit();

	bool isPlaying() const;
	const glm::mat4& getSampledCornerColors() const;

	virtual void updateAsynchronous(float deltaSeconds) override;

private:
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
	int m_FrameCount;
	int m_CurrentFrame;
	unsigned char* m_PixelData;
	std::atomic_bool m_HasUpdate;
	std::atomic_bool m_Playing;
};
