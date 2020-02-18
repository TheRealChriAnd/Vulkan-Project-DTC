#include "CommandPoolVK.h"
#include "DeviceVK.h"

CommandPoolVK::CommandPoolVK(DeviceVK* device)
{
	m_Device = device;
	QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex	= queueFamilyIndices.m_GraphicsFamily.value();
	poolInfo.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create graphics command pool!");
}

CommandPoolVK::~CommandPoolVK()
{
	vkDestroyCommandPool(m_Device->getDevice(), m_CommandPool, nullptr);
}
