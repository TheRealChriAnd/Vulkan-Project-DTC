#pragma once
#include "Defines.h"
#include <vector>

class DeviceVK;
class SwapChainVK;
class DescriptorSetLayoutVK;
class UniformBufferVK;
class TextureVK;
class SamplerVK;
class StorageBufferVK;

class DescriptorSetVK
{
public:
	DescriptorSetVK(DeviceVK* device, SwapChainVK* swapChain, DescriptorSetLayoutVK* layout);
	~DescriptorSetVK();

	VkDescriptorSet getDescriptorSet(int index) const;

	void addUniformBuffer(uint32_t binding, UniformBufferVK* uniformBuffer);
	void addTexture(uint32_t binding, TextureVK* texture, SamplerVK* sampler);
	void addStorageBuffer(uint32_t binding, StorageBufferVK* storagebuffer, size_t range, size_t offset);

	void submit();

private:
	std::vector<VkDescriptorSet> m_DescriptorSets;
	std::vector<VkWriteDescriptorSet> m_PendingWrites;

	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
};
