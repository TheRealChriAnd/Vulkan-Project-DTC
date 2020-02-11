#include "StorageBufferVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include <iostream>

StorageBufferVK::StorageBufferVK(DeviceVK* device) :
	m_pBuffer(nullptr)
{
	m_Device = device;
}

StorageBufferVK::~StorageBufferVK()
{
	delete m_pBuffer;
}

void StorageBufferVK::setData(const void* data, size_t size, size_t offset)
{
	m_Size = size;

	BufferVK stagingBuffer(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.writeData(data, m_Size);

	m_pBuffer = new BufferVK(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	stagingBuffer.copyToBuffer(m_pBuffer, m_Size);
}

BufferVK* StorageBufferVK::getBuffer() const
{
	return m_pBuffer;
}

VkBuffer StorageBufferVK::getVKBuffer() const
{
	return m_pBuffer->getBuffer();
}

size_t StorageBufferVK::getSize() const
{
	return m_Size;
}
