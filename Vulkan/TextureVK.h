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
	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount = 1);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, const std::vector<VkBufferImageCopy>& regions);

protected:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};
