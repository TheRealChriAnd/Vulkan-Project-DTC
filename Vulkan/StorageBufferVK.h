#pragma once
#include "Defines.h"
#include "PipelineVK.h"
#include <vector>

class DeviceVK;
class BufferVK;


class StorageBufferVK
{
public:
	StorageBufferVK(DeviceVK* device);
	~StorageBufferVK();

	void setData(const void* data, size_t size, size_t offset);
	size_t getSize() const;

	BufferVK* getBuffer() const;
	VkBuffer getVKBuffer() const;

private:
	BufferVK* m_pBuffer;
	VkDeviceMemory m_BufferMemory;
	VkDeviceSize m_Size;

	DeviceVK* m_Device;
};
