#pragma once
#include "Defines.h"
#include <string>

class DeviceVK;
class BufferVK;

class TextureVK
{
public:
	TextureVK(DeviceVK* device);
	~TextureVK();

	virtual int loadFromFile(std::string filename);
	virtual void bind(unsigned int slot);

	VkImageView getImageView() const;

private:
	void createTextureImage(DeviceVK* device, const std::string& file);
	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height);

private:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};
