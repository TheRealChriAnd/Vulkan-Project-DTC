#pragma once
#include "Defines.h"
#include <vector>
#include <set>

class DeviceVK;
class SwapChainVK;
class BufferVK;

class UniformBufferVK
{
	friend class DescriptorSetVK;
	friend class Application;
public:
	UniformBufferVK(DeviceVK* device, SwapChainVK* swapChain, uint32_t size);
	~UniformBufferVK();

	int getSize() const;
	void setData(const void* data);

private:
	VkBuffer getVKBuffer() const;
	static void transfer();

private:
	BufferVK* m_UniformBuffer;
	std::vector<BufferVK*> m_Buffers;
	int m_Size;
	int bufferUpdateIndex;

	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
	static std::set<UniformBufferVK*> m_UniformBuffers;
};
