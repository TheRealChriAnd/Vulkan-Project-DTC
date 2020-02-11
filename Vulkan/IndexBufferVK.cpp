#include "IndexBufferVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include <iostream>

IndexBufferVK::IndexBufferVK(DeviceVK* device, const std::vector<uint16_t>& indices) : m_pBuffer(nullptr)
{
	m_Device = device;

	createIndexBuffer(device, indices);
}

IndexBufferVK::~IndexBufferVK()
{
	delete m_pBuffer;
}

VkIndexType IndexBufferVK::getIndexType() const
{
	return VK_INDEX_TYPE_UINT16;
}

BufferVK* IndexBufferVK::getBuffer() const
{
	return m_pBuffer;
}

VkBuffer IndexBufferVK::getVKBuffer() const
{
	return m_pBuffer->getBuffer();
}

int IndexBufferVK::getCount() const
{
	return m_Count;
}

void IndexBufferVK::createIndexBuffer(DeviceVK* device, const std::vector<uint16_t>& indices)
{
	m_Count = indices.size();
	VkDeviceSize bufferSize = sizeof(indices[0]) * m_Count;

	BufferVK stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.writeData(indices.data(), (size_t)bufferSize);

	m_pBuffer = new BufferVK(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	stagingBuffer.copyToBuffer(m_pBuffer, bufferSize);
}