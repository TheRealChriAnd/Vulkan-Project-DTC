#include "UniformBufferVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "SwapChainVK.h"
#include <iostream>

#define UP_TO_DATE -1

std::set<UniformBufferVK*> UniformBufferVK::m_UniformBuffers;

UniformBufferVK::UniformBufferVK(DeviceVK* device, SwapChainVK* swapChain, uint32_t size) : bufferUpdateIndex(UP_TO_DATE)
{
	m_UniformBuffers.insert(this);

	m_Device = device;
	m_SwapChain = swapChain;
	m_Size = size;

	m_Buffers.resize(swapChain->getCount());

	m_UniformBuffer = new BufferVK(m_Device, m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	for (int i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_Buffers[i] = new BufferVK(m_Device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

UniformBufferVK::~UniformBufferVK()
{
	delete m_UniformBuffer;
	for (BufferVK* buffer : m_Buffers)
	{
		delete buffer;
	}
	m_UniformBuffers.erase(this);
}

int UniformBufferVK::getSize() const
{
	return m_Size;
}

void UniformBufferVK::setData(const void* data)
{
	bufferUpdateIndex = m_SwapChain->getCurrentImageIndex();
	m_Buffers[bufferUpdateIndex]->writeData(data, m_Size);
}

VkBuffer UniformBufferVK::getVKBuffer() const
{
	return m_UniformBuffer->getBuffer();
}

void UniformBufferVK::transfer()
{
	for (UniformBufferVK* ub : m_UniformBuffers)
	{
		if (ub->bufferUpdateIndex != UP_TO_DATE)
		{
			ub->m_Buffers[ub->bufferUpdateIndex]->copyToBuffer(ub->m_UniformBuffer, ub->m_Size);
			ub->bufferUpdateIndex = UP_TO_DATE;
		}
	}
}