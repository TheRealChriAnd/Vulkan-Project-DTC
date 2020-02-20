#pragma once
#include "Defines.h"

class DeviceVK;

class CommandPoolVK
{
	friend class CommandBufferVK;

public: 
	CommandPoolVK(DeviceVK* device, bool isTransfer);
	~CommandPoolVK();

private:
	VkCommandPool m_CommandPool;
	DeviceVK* m_Device;
};