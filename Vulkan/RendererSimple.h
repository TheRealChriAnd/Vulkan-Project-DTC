#pragma once
#include "IRenderer.h"

class DescriptorSetLayoutVK;
class TextureVK;
class SamplerVK;
class GameObjectSimple;
class DescriptorSetVK;
class UniformBufferVK;
class CameraVK;

class RendererSimple : public IRenderer
{
public:
	RendererSimple(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	virtual ~RendererSimple();

	virtual void init() override;
	virtual void render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects) override;

	GameObjectSimple* createGameObject(Mesh* mesh, TextureVK* texture, SamplerVK* smapler);

	void test(CameraVK* camera);

protected:
	DescriptorSetVK* m_DescriptorSetPerFrame;
	DescriptorSetLayoutVK* m_LayoutObject;
	UniformBufferVK* m_UniformBuffer;
};