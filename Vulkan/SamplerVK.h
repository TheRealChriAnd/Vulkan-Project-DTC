#pragma once
#include "Defines.h"

class DeviceVK;

class SamplerVK
{
public:
	SamplerVK(DeviceVK* device);
	~SamplerVK();

	VkSampler getSampler() const;

private:
	void createTextureSampler(DeviceVK* device, VkSamplerAddressMode modeU, VkSamplerAddressMode modeV);

private:
	VkSampler m_Sampler;

	DeviceVK* m_Device;
};
