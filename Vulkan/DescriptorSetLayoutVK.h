#pragma once
#include "Defines.h"
#include <vector>

class DeviceVK;

class DescriptorSetLayoutVK
{
public:
	DescriptorSetLayoutVK(DeviceVK* device);
	~DescriptorSetLayoutVK();

	VkDescriptorSetLayout getDescriptorSetLayout() const;

	void addUniformBuffer(uint16_t binding, VkShaderStageFlags shaderStages);
	void addTexture(uint16_t binding, VkShaderStageFlagBits shaderStages);
	void addStorageBuffer(uint16_t binding, VkShaderStageFlags shaderStages);
	void submit();

private:
	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings;
	DeviceVK* m_Device;
};