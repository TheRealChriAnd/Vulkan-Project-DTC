#pragma once
#include "Defines.h"
#include "PipelineVK.h"
#include <vector>

class DeviceVK;
class BufferVK;


class VertexBufferVK
{
public:
	VertexBufferVK(DeviceVK* device);
	~VertexBufferVK();

	virtual void setData(const void* data, size_t size, size_t offset);
	virtual void bind(size_t offset, size_t size, unsigned int location);
	virtual void unbind();
	virtual size_t getSize();

	BufferVK* getBuffer() const;
	VkBuffer getVKBuffer() const;

private:
	void createVertexBuffer(DeviceVK* device, const void* data, size_t size, int offset, VkBufferUsageFlagBits bufferUsage);

private:
	BufferVK* m_pBuffer;
	VkDeviceMemory m_BufferMemory;
	VkDeviceSize m_Size;

	void* m_Data;

	size_t m_Offset;

	DeviceVK* m_Device;
};
