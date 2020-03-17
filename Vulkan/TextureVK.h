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
	void transitionImageLayout(VkImage image, uint32_t src, uint32_t dst, uint32_t layerCount = 1);
	void acquireFromQueue(bool transfer, VkImage image, uint32_t layerCount = 1);
	void releaseFromQueue(bool transfer, VkImage image, uint32_t layerCount = 1);
	void copyBufferToImage(DeviceVK* device, VkImage image, bool transferQueue, const BufferVK& buffer, uint32_t width, uint32_t height, const std::vector<VkBufferImageCopy>& regions);
	void transfer(VkImage image, BufferVK* stagingBuffer, int height, int width, const std::vector<VkBufferImageCopy>& region, uint32_t layerCount = 1);

protected:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};