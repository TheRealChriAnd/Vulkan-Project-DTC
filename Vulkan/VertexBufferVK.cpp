#include "VertexBufferVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include <iostream>

VertexBufferVK::VertexBufferVK(DeviceVK* device) :
	m_pBuffer(nullptr),
	m_Data(nullptr)
{
	m_Device = device;
}

VertexBufferVK::~VertexBufferVK()
{
	m_pBuffer->release();
	vkFreeMemory(m_Device->getDevice(), m_BufferMemory, nullptr);

	delete[] m_Data;
}

void VertexBufferVK::setData(const void* data, size_t size, size_t offset)
{
	createVertexBuffer(m_Device, data, size, offset, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

void VertexBufferVK::bind(size_t offset, size_t size, unsigned int location)
{
}

void VertexBufferVK::unbind()
{
}

BufferVK* VertexBufferVK::getBuffer() const
{
	return m_pBuffer;
}

VkBuffer VertexBufferVK::getVKBuffer() const
{
	return m_pBuffer->getBuffer();
}

size_t VertexBufferVK::getSize()
{
	return m_Size;
}

void VertexBufferVK::createVertexBuffer(DeviceVK* device, const void* data, size_t size, int offset, VkBufferUsageFlagBits bufferUsage)
{
	size_t alignment = device->getProperties().limits.minStorageBufferOffsetAlignment;
	size_t alignedOffset = offset + (-offset & (alignment - 1));
	size_t newSize = size + alignedOffset;

	void* newData = new char[newSize];
	if (m_Data)
	{
		m_pBuffer->release();
		vkFreeMemory(m_Device->getDevice(), m_BufferMemory, nullptr);

		memcpy(newData, m_Data, m_Size);
		delete[] m_Data;
	}

	memcpy((void*)((char*)newData + alignedOffset), data, size);
	m_Data = newData;
	m_Size = newSize;

	VkDeviceMemory stagingBufferMemory = nullptr;
	BufferVK* stagingBuffer = new BufferVK(device, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	void* destinationBuffer = nullptr;
	vkMapMemory(device->getDevice(), stagingBufferMemory, 0, m_Size, 0, &destinationBuffer);
	memcpy(destinationBuffer, m_Data, m_Size);
	vkUnmapMemory(device->getDevice(), stagingBufferMemory);

	m_pBuffer = new BufferVK(device, m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

	stagingBuffer->copyToBuffer(device,  m_pBuffer, m_Size);

	stagingBuffer->release();
	vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
}
