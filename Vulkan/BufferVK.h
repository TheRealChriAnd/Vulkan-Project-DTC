#pragma once
#include "Defines.h"

class DeviceVK;

class BufferVK
{
public:
	BufferVK(DeviceVK* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~BufferVK();

	void writeData(const void* data, uint32_t length);

	void copyToBuffer(BufferVK* dstBuffer, VkDeviceSize size);
	VkBuffer getBuffer() const;

private:
	static void createBuffer(DeviceVK* device, VkBuffer& m_Buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
	
private:
	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;

	DeviceVK* m_Device;
};
