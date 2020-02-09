#pragma once
#include "Defines.h"
#include <vector>

class DeviceVK;
class SwapChainVK;
class MaterialVK;

class UniformBufferVK
{
public:
	UniformBufferVK(DeviceVK* device, SwapChainVK* swapChain);
	~UniformBufferVK();

	int getSize() const;
	void setBufferSize(int size);
	void setData(const void* data, bool updateAll = false);
	VkDeviceMemory getBufferMemory(int index) const;
	const std::vector<VkBuffer>& getBuffers() const;
	VkBuffer getBuffer(int index) const;
	void setData(const void* data, size_t size);

private:
	void writeToBuffer(const void* data, int index);

private:
	std::vector<VkBuffer> m_Buffers;
	std::vector<VkDeviceMemory> m_BufferMemories;
	int m_Size;

	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
};
