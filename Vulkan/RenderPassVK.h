#pragma once
#include "Defines.h"
#include <vector>

class DeviceVK;
class SwapChainVK;

class RenderPassVK
{
public:
	RenderPassVK(DeviceVK* device, SwapChainVK* swapChain);
	~RenderPassVK();

	VkRenderPass getRenderPass() const;

	void release();

private:
	void createRenderPass(DeviceVK* device, SwapChainVK* swapChain);

private:
	VkRenderPass m_RenderPass;

	DeviceVK* m_Device;
};