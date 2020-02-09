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
	vkFreeMemory(m_Device->getDevice(), m_BufferMemory, nullptr);
}

void StorageBufferVK::setData(const void* data, size_t size, size_t offset)
{
	m_Size = size;

	VkDeviceMemory stagingBufferMemory = nullptr;
	BufferVK stagingBuffer(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	void* destinationBuffer = nullptr;
	vkMapMemory(m_Device->getDevice(), stagingBufferMemory, 0, m_Size, 0, &destinationBuffer);
	memcpy(destinationBuffer, data, m_Size);
	vkUnmapMemory(m_Device->getDevice(), stagingBufferMemory);

	m_pBuffer = new BufferVK(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

	stagingBuffer.copyToBuffer(m_Device, m_pBuffer, m_Size);
	vkFreeMemory(m_Device->getDevice(), stagingBufferMemory, nullptr);
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
