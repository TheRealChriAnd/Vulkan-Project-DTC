#include "UniformBufferVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "SwapChainVK.h"
#include "MaterialVK.h"
#include <iostream>

UniformBufferVK::UniformBufferVK(DeviceVK* device, SwapChainVK* swapChain) : m_Size(0)
{
	m_Device = device;
	m_SwapChain = swapChain;

	m_Buffers.resize(swapChain->getCount());
	m_BufferMemories.resize(swapChain->getCount());
}

UniformBufferVK::~UniformBufferVK()
{
	VkDevice device = m_Device->getDevice();
	for (size_t i = 0; i < m_Buffers.size(); i++)
	{
		vkDestroyBuffer(device, m_Buffers[i], nullptr);
		vkFreeMemory(device, m_BufferMemories[i], nullptr);
	}
}

int UniformBufferVK::getSize() const
{
	return m_Size;
}

void UniformBufferVK::setBufferSize(int size)
{
	if (m_Size != size)
	{
		m_Size = size;
		for (int i = 0; i < m_SwapChain->getCount(); i++)
		{
			BufferVK::createBuffer(m_Device, m_Buffers[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_BufferMemories[i]);
		}
	}
}

void UniformBufferVK::setData(const void* data, bool updateAll)
{
	if (updateAll)
	{
		for (int i = 0; i < m_Buffers.size(); i++)
			writeToBuffer(data, i);
	}
	else
	{
		writeToBuffer(data, m_SwapChain->getCurrentImageIndex());
	}
}

VkDeviceMemory UniformBufferVK::getBufferMemory(int index) const
{
	return m_BufferMemories[index];
}

const std::vector<VkBuffer>& UniformBufferVK::getBuffers() const
{
	return m_Buffers;
}

VkBuffer UniformBufferVK::getBuffer(int index) const
{
	return m_Buffers[index];
}

void UniformBufferVK::setData(const void* data, size_t size)
{
	setBufferSize(size);
	setData(data);
}

void UniformBufferVK::writeToBuffer(const void* data, int index)
{
	void* destinationBuffer;
	VkDeviceMemory memory = getBufferMemory(index);
	vkMapMemory(m_Device->getDevice(), memory, 0, m_Size, 0, &destinationBuffer);
	memcpy(destinationBuffer, data, m_Size);
	vkUnmapMemory(m_Device->getDevice(), memory);
}
