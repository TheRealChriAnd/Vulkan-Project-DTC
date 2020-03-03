#include "BufferVK.h"
#include "DeviceVK.h"
#include "CommandBufferVK.h"
#include <iostream>

BufferVK::BufferVK(DeviceVK* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : m_Buffer(nullptr)
{
	m_Device = device;
	createBuffer(device, m_Buffer, size, usage, properties, m_Memory);
}

BufferVK::~BufferVK()
{
	vkDestroyBuffer(m_Device->getDevice(), m_Buffer, nullptr);
	vkFreeMemory(m_Device->getDevice(), m_Memory, nullptr);
}

void BufferVK::writeData(const void* data, uint32_t length)
{
	void* destinationBuffer = nullptr;
	vkMapMemory(m_Device->getDevice(), m_Memory, 0, length, 0, &destinationBuffer);
	memcpy(destinationBuffer, data, length);
	vkUnmapMemory(m_Device->getDevice(), m_Memory);
}

VkBuffer BufferVK::getBuffer() const
{
	return m_Buffer;
}

void BufferVK::createBuffer(DeviceVK* device, VkBuffer& m_Buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size;
	bufferInfo.usage					= usage;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->getDevice(), m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= device->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to allocate buffer memory!");

	vkBindBufferMemory(device->getDevice(), m_Buffer, bufferMemory, 0);
}

void BufferVK::copyToBuffer(BufferVK* dstBuffer, VkDeviceSize size)
{
	CommandBufferVK commandBuffer(m_Device, true);
	commandBuffer.begin();
	commandBuffer.copyBuffer(this, dstBuffer, size);
	commandBuffer.end();
	commandBuffer.submit();
}
