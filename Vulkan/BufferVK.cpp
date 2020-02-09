#include "BufferVK.h"
#include "DeviceVK.h"
#include "CommandBufferVK.h"
#include <iostream>

BufferVK::BufferVK(DeviceVK* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory) : m_Buffer(nullptr)
{
	m_Device = device;

	createBuffer(device, m_Buffer, size, usage, properties, bufferMemory);
}

BufferVK::~BufferVK()
{
	vkDestroyBuffer(m_Device->getDevice(), m_Buffer, nullptr);
}

VkBuffer BufferVK::getBuffer() const
{
	return m_Buffer;
}

void BufferVK::createBuffer(DeviceVK* device, VkBuffer& m_Buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size			= size;
	bufferInfo.usage		= usage;
	bufferInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

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

void BufferVK::copyToBuffer(DeviceVK* device, BufferVK* dstBuffer, VkDeviceSize size)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer.getCommandBuffer(), m_Buffer, dstBuffer->getBuffer(), 1, &copyRegion);

	commandBuffer.end();
	commandBuffer.submit();
}
