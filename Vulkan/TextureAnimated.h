#pragma once
#include "TextureVK.h"
#include <string>
#include <atomic>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SpinLock.h"
#include "IVideoSourceListener.h"
#include "IAsynchronous.h"

class BufferVK;

class TextureAnimated : public TextureVK, public IVideoSourceListener, public IAsynchronous
{
public:
	TextureAnimated(DeviceVK* device, VideoSource* source);
	virtual ~TextureAnimated();

	VkImageView getImageView2() const;
	VkImage getImage2() const;
	VkDeviceMemory getDeviceMemory2() const;

	bool useFirstImage() const;
	void update(float deltaSeconds);

	int getHeight() const;
	int getWidth() const;

	void setOnFrameReadyCallback(const std::function<void(TextureAnimated*)>& function);
	glm::u8vec3 getColor(int x, int y) const;
	glm::vec3 getSampleColor(int width, int height, int x, int y, int sampleEvery) const;

	virtual void onFrameReady(VideoSource* source) override;
	virtual void updateAsynchronous(float deltaSeconds) override;

	static int getLostFrames();

private:
	BufferVK* m_StagingBuffer;
	VideoSource* m_Source;
	VkBufferImageCopy m_Region;

	std::atomic_bool m_HasUpdate;
	std::atomic_bool m_HasUnWritenData;
	std::function<void(TextureAnimated*)> m_OnFrameReadyCallback;

	VkImage m_Image2;
	VkImageView m_ImageView2;
	VkDeviceMemory m_ImageMemory2;
	std::atomic_bool m_UseFirstImage;
	SpinLock m_Lock;
	int m_Id;

	static int m_Counter;
	static std::atomic_int m_LostFrames;
};
