#pragma once
#include "IRenderer.h"
#include <glm/glm.hpp>

class DescriptorSetLayoutVK;
class TextureVK;
class SamplerVK;
class GameObjectSimple;
class DescriptorSetVK;
class UniformBufferVK;
class CameraVK;
class PipelineVK;
class Mesh;
class ShaderVK;

class RendererSimple : public IRenderer
{
public:
	RendererSimple(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	virtual ~RendererSimple();

	virtual void init() override;
	virtual void render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects) override;

	GameObjectSimple* createGameObject(Mesh* mesh, TextureVK* texture, SamplerVK* smapler);

	virtual void update(float deltaSeconds, CameraVK* camera) override;

protected:
	PipelineVK* m_Pipeline;
	DescriptorSetVK* m_DescriptorSetPerFrame;
	DescriptorSetLayoutVK* m_LayoutCamera;
	DescriptorSetLayoutVK* m_LayoutObject;
	DescriptorSetLayoutVK* m_LayoutRayTracing;
	UniformBufferVK* m_UniformBuffer;
	ShaderVK* m_ShaderFragment;
	ShaderVK* m_ShaderVertex;
	ShaderVK* m_ShaderRaygen;
	ShaderVK* m_ShaderMiss;
	ShaderVK* m_ShaderChit;

private:
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;

		glm::vec4 dir;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

		glm::vec4 camPos;
	};

};