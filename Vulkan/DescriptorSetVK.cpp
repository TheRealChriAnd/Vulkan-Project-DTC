#include "DescriptorSetVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "DescriptorSetLayoutVK.h"
#include "TextureVK.h"
#include "UniformBufferVK.h"
#include "SamplerVK.h"
#include "StorageBufferVK.h"
#include <array>

DescriptorSetVK::DescriptorSetVK(DeviceVK* device, SwapChainVK* swapChain, DescriptorSetLayoutVK* layout)
{
	m_Device = device;
	m_SwapChain = swapChain;
	m_DescriptorSets.resize(swapChain->getCount());

	std::vector<VkDescriptorSetLayout> layouts(swapChain->getCount(), layout->getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool		= device->getDescriptorPool();
	allocInfo.descriptorSetCount	= static_cast<uint32_t>(swapChain->getCount());
	allocInfo.pSetLayouts			= layouts.data();

	if (vkAllocateDescriptorSets(device->getDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to allocate descriptor sets!");
}

DescriptorSetVK::~DescriptorSetVK()
{

}

VkDescriptorSet DescriptorSetVK::getDescriptorSet(int index) const
{
	return m_DescriptorSets[index];
}

void DescriptorSetVK::addUniformBuffer(uint32_t binding, UniformBufferVK* uniformBuffer)
{
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer	= uniformBuffer->getBuffer(i);
		bufferInfo->offset	= 0;
		bufferInfo->range	= uniformBuffer->getSize();

		VkWriteDescriptorSet descriptorWriter = {};
		descriptorWriter.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriter.dstSet				= m_DescriptorSets[i];
		descriptorWriter.dstBinding			= binding;
		descriptorWriter.dstArrayElement	= 0;
		descriptorWriter.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWriter.descriptorCount	= 1;
		descriptorWriter.pBufferInfo		= bufferInfo;
		descriptorWriter.pImageInfo			= nullptr;

		m_PendingWrites.push_back(descriptorWriter);
	}
}

void DescriptorSetVK::addTexture(uint32_t binding, TextureVK* texture, SamplerVK* sampler)
{
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->imageView	= texture->getImageView();
		imageInfo->sampler		= sampler->getSampler();

		VkWriteDescriptorSet descriptorWriter = {};
		descriptorWriter.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriter.dstSet				= m_DescriptorSets[i];
		descriptorWriter.dstBinding			= binding;
		descriptorWriter.dstArrayElement	= 0;
		descriptorWriter.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWriter.descriptorCount	= 1;
		descriptorWriter.pImageInfo			= imageInfo;
		descriptorWriter.pBufferInfo		= nullptr;

		m_PendingWrites.push_back(descriptorWriter);
	}
}

void DescriptorSetVK::addStorageBuffer(uint32_t binding, StorageBufferVK* storagebuffer, size_t range, size_t offset)
{
	for(size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer	= storagebuffer->getVKBuffer();
		bufferInfo->offset	= offset;
		bufferInfo->range	= range;

		VkWriteDescriptorSet descriptorWriter = {};
		descriptorWriter.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriter.dstSet				= m_DescriptorSets[i];
		descriptorWriter.dstBinding			= binding;
		descriptorWriter.dstArrayElement	= 0;
		descriptorWriter.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWriter.descriptorCount	= 1;
		descriptorWriter.pBufferInfo		= bufferInfo;
		descriptorWriter.pImageInfo			= nullptr;

		m_PendingWrites.push_back(descriptorWriter);
	}
}

void DescriptorSetVK::submit()
{
	vkUpdateDescriptorSets(m_Device->getDevice(), static_cast<uint32_t>(m_PendingWrites.size()), m_PendingWrites.data(), 0, nullptr);

	for (VkWriteDescriptorSet& write : m_PendingWrites)
	{
		if (write.pBufferInfo)
			delete write.pBufferInfo;
		else if (write.pImageInfo)
			delete write.pImageInfo;
	}

	m_PendingWrites.clear();
}