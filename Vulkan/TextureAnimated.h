#pragma once
#include "TextureVK.h"
#include <string>
#include <atomic>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "IAsynchronous.h"
#include "SpinLock.h"

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

	VkImageView getImageView2() const;
	VkImage getImage2() const;
	VkDeviceMemory getDeviceMemory2() const;

	bool useFirstImage() const;

	void play();
	void stop();
	void submit();

	int getHeight() const;
	int getWidth() const;

	bool isPlaying() const;
	void setOnFrameReadyCallback(const std::function<void(TextureAnimated*)>& function);
	glm::u8vec3 getColor(int x, int y) const;
	glm::vec3 getSampleColor(int width, int height, int x, int y) const;

	virtual void updateAsynchronous(float deltaSeconds) override;

private:
	void update(float deltaSeconds);
	void onFrameReady();

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
	std::function<void(TextureAnimated*)> m_OnFrameReadyCallback;

	VkImage m_Image2;
	VkImageView m_ImageView2;
	VkDeviceMemory m_ImageMemory2;
	std::atomic_bool m_UseFirstImage;
	SpinLock m_Lock;
};
