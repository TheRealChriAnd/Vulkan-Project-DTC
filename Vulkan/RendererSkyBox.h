#pragma once
#include "IRenderer.h"
#include <glm/glm.hpp>

class DescriptorSetLayoutVK;
class GameObjectSkyBox;
class DescriptorSetVK;
class UniformBufferVK;
class CameraVK;
class PipelineVK;
class Mesh;
class ShaderVK;
class TextureSkyBox;
class SamplerVK;

class RendererSkyBox : public IRenderer
{
public:
	RendererSkyBox(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	virtual ~RendererSkyBox();

	virtual void init() override;
	virtual void render(CommandBufferVK* m_CommandBuffer, int index, const std::vector<GameObject*>& gameObjects) override;

	GameObjectSkyBox* createGameObject(Mesh* mesh, TextureSkyBox* texture, SamplerVK* sampler);

	virtual void update(float deltaSeconds, CameraVK* camera) override;

protected:
	PipelineVK* m_Pipeline;
	DescriptorSetVK* m_DescriptorSetPerFrame;
	DescriptorSetLayoutVK* m_LayoutObject;
	UniformBufferVK* m_UniformBuffer;
	ShaderVK* m_ShaderFragment;
	ShaderVK* m_ShaderVertex;

private: 
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 camPos;
	};
};