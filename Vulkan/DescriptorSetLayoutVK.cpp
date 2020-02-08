#include "DescriptorSetLayoutVK.h"
#include "DeviceVK.h"
#include <array>
#include <iostream>

DescriptorSetLayoutVK::DescriptorSetLayoutVK(DeviceVK* device) : m_DescriptorSetLayout(nullptr)
{
	m_Device = device;

}

DescriptorSetLayoutVK::~DescriptorSetLayoutVK()
{
	if (m_DescriptorSetLayout)
		std::cout << "DescriptorSetLayoutVK not released!";
}

VkDescriptorSetLayout DescriptorSetLayoutVK::getDescriptorSetLayout() const
{
	return m_DescriptorSetLayout;
}

void DescriptorSetLayoutVK::addUniformBuffer(uint16_t binding, VkShaderStageFlags shaderStages)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding			= binding;
	uboLayoutBinding.descriptorCount	= 1;
	uboLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags			= shaderStages;

	m_LayoutBindings.push_back(uboLayoutBinding);
}

void DescriptorSetLayoutVK::addTexture(uint16_t binding, VkShaderStageFlagBits shaderStages)
{
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding			= binding;
	samplerLayoutBinding.descriptorCount	= 1;
	samplerLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags			= shaderStages;

	m_LayoutBindings.push_back(samplerLayoutBinding);
}

void DescriptorSetLayoutVK::addStorageBuffer(uint16_t binding, VkShaderStageFlags shaderStages)
{
	VkDescriptorSetLayoutBinding storageLayoutBinding = {};
	storageLayoutBinding.binding			= binding;
	storageLayoutBinding.descriptorCount	= 1;
	storageLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	storageLayoutBinding.pImmutableSamplers = nullptr;
	storageLayoutBinding.stageFlags			= shaderStages;

	m_LayoutBindings.push_back(storageLayoutBinding);
}

void DescriptorSetLayoutVK::submit()
{
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(m_LayoutBindings.size());
	layoutInfo.pBindings = m_LayoutBindings.data();

	if (vkCreateDescriptorSetLayout(m_Device->getDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create descriptor set layout!");

	m_LayoutBindings.clear();
}

void DescriptorSetLayoutVK::release()
{
	vkDestroyDescriptorSetLayout(m_Device->getDevice(), m_DescriptorSetLayout, nullptr);
	m_DescriptorSetLayout = nullptr;

	delete this;
}