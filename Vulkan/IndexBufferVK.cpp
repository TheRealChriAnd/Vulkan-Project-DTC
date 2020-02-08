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
	if (m_pBuffer)
		std::cout << "IndexBufferVK not released!";
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

void IndexBufferVK::release()
{
	m_pBuffer->release();
	vkFreeMemory(m_Device->getDevice(), m_BufferMemory, nullptr);

	m_pBuffer = nullptr;

	delete this;
}

void IndexBufferVK::createIndexBuffer(DeviceVK* device, const std::vector<uint16_t>& indices)
{
	m_Count = indices.size();
	VkDeviceSize bufferSize = sizeof(indices[0]) * m_Count;

	VkDeviceMemory stagingBufferMemory = nullptr;
	BufferVK* stagingBuffer = new BufferVK(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	void* data = nullptr;
	vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device->getDevice(), stagingBufferMemory);

	m_pBuffer = new BufferVK(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_BufferMemory);

	stagingBuffer->copyToBuffer(device, m_pBuffer, bufferSize);

	stagingBuffer->release();
	vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
}