#include "SamplerVK.h"
#include "DeviceVK.h"
#include <iostream>

SamplerVK::SamplerVK(DeviceVK* device)
{
	m_Device = device;
	createTextureSampler(m_Device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

SamplerVK::~SamplerVK()
{
	vkDestroySampler(m_Device->getDevice(), m_Sampler, nullptr);
}

VkSampler SamplerVK::getSampler() const
{
	return m_Sampler;
}

void SamplerVK::createTextureSampler(DeviceVK* device, VkSamplerAddressMode modeU, VkSamplerAddressMode modeV)
{
	VkSamplerCreateInfo samplerInfo		= {};
	samplerInfo.sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter				= VK_FILTER_LINEAR;
	samplerInfo.minFilter				= VK_FILTER_LINEAR;
	samplerInfo.addressModeU			= modeU;
	samplerInfo.addressModeV			= modeV;
	samplerInfo.addressModeW			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable		= VK_TRUE;
	samplerInfo.maxAnisotropy			= 16;
	samplerInfo.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable			= VK_FALSE;
	samplerInfo.compareOp				= VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create texture sampler!");
}
