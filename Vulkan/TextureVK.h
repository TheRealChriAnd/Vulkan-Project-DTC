#pragma once
#include "Defines.h"

class DeviceVK;

class TextureVK
{
public:
	TextureVK(DeviceVK* device);
	virtual ~TextureVK();

	VkImageView getImageView() const;
	VkImage getImage() const;
	VkDeviceMemory getDeviceMemory() const;

private:

protected:
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DeviceVK* m_Device;
};
