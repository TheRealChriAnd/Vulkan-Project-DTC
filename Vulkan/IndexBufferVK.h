#pragma once
#include "Defines.h"
#include "PipelineVK.h"
#include <vector>

class DeviceVK;
class BufferVK;

class IndexBufferVK
{
public:
	IndexBufferVK(DeviceVK* device, const std::vector<uint16_t>& indices);
	~IndexBufferVK();

	VkIndexType getIndexType() const;
	BufferVK* getBuffer() const;
	VkBuffer getVKBuffer() const;
	int getCount() const;

private:
	void createIndexBuffer(DeviceVK* device, const std::vector<uint16_t>& indices);

private:
	BufferVK* m_pBuffer;
	int m_Count;

	DeviceVK* m_Device;
};
