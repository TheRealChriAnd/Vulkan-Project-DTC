#pragma once
#include <vector>

class DeviceVK;
class Mesh;
class DescriptorSetLayoutVK;
class PipelineVK;
class SwapChainVK;
class RenderPassVK;
class CommandBufferVK;
class GameObject;

class IRenderer
{
public:
	IRenderer(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	virtual ~IRenderer();

	virtual void init() = 0;

	virtual void render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects) = 0;

protected:
	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
	RenderPassVK* m_RenderPass;
	PipelineVK* m_Pipeline;
};