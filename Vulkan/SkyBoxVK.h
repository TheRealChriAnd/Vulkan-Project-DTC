#pragma once
#include "Defines.h"
#include <string>

class DeviceVK;
class BufferVK;

class SkyBoxVK
{
public:
	SkyBoxVK(DeviceVK* device);
	~SkyBoxVK();

	virtual int loadFromFile(std::string filename, int index);
	virtual void bind(unsigned int slot);

	VkImageView getImageView() const;

private:
	void createTextureImage(DeviceVK* device, const std::string& file, uint32_t layers, int index);
	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int index);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, int index);

private:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};
