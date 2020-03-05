#pragma once
#include "Defines.h"
#include <vector>

class DeviceVK;
class BufferVK;

class TextureVK
{
public:
	TextureVK(DeviceVK* device);
	virtual ~TextureVK();

	VkImageView getImageView() const;
	VkImage getImage() const;
	VkDeviceMemory getDeviceMemory() const;

protected:
	void transitionImageLayout(uint32_t src, uint32_t dst, uint32_t layerCount = 1);
	void acquireFromQueue(uint32_t src, uint32_t dst, uint32_t layerCount = 1);
	void releaseFromQueue(uint32_t src, uint32_t dst, uint32_t layerCount = 1);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, const std::vector<VkBufferImageCopy>& regions);

	void transfer(BufferVK* stagingBuffer, int height, int width, const std::vector<VkBufferImageCopy>& region, uint32_t layerCount = 1);

protected:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};