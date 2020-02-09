#pragma once
#include "Defines.h"

class DeviceVK;

class BufferVK
{
	friend class UniformBufferVK;

public:
	BufferVK(DeviceVK* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
	~BufferVK();

	void copyToBuffer(DeviceVK* device, BufferVK* dstBuffer, VkDeviceSize size);
	VkBuffer getBuffer() const;

private:
	static void createBuffer(DeviceVK* device, VkBuffer& m_Buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
	
private:
	VkBuffer m_Buffer;

	DeviceVK* m_Device;
};
