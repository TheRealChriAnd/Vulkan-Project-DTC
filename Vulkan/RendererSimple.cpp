#include "RendererSimple.h"
#include "PipelineVK.h"
#include "ShaderVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "CommandBufferVK.h"
#include "DescriptorSetLayoutVK.h"
#include "DescriptorSetVK.h"
#include "UniformBufferVK.h"
#include "Mesh.h"
#include "GameObjectSimple.h"
#include "CameraVK.h"

#define BINDING_POS 0
#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_UBO 3
#define BINDING_TEX 4

struct UniformBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

RendererSimple::RendererSimple(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass) : IRenderer(device, swapChain, renderPass)
{

}

RendererSimple::~RendererSimple()
{

}

void RendererSimple::init()
{
	DescriptorSetLayoutVK* perFrameLayout = new DescriptorSetLayoutVK(m_Device);
	perFrameLayout->addUniformBuffer(0, VK_SHADER_STAGE_VERTEX_BIT);
	perFrameLayout->submit();

	m_LayoutObject = new DescriptorSetLayoutVK(m_Device);
	m_LayoutObject->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_NOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_UVS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addUniformBuffer(BINDING_UBO, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addTexture(BINDING_TEX, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_LayoutObject->submit();

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(UniformBufferObject));

	m_DescriptorSetPerFrame = new DescriptorSetVK(m_Device, m_SwapChain, perFrameLayout);
	m_DescriptorSetPerFrame->addUniformBuffer(0, m_UniformBuffer);
	m_DescriptorSetPerFrame->submit();

	ShaderVK* vertexShader = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	vertexShader->compile();

	ShaderVK* fragmentShader = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	fragmentShader->compile();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(perFrameLayout);
	m_Pipeline->addDescriptorLayout(m_LayoutObject);
	m_Pipeline->addShader(vertexShader);
	m_Pipeline->addShader(fragmentShader);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);
}

void RendererSimple::render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects)
{
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->bindPipeline(i, m_Pipeline);
		for (GameObject* gameObject : gameObjects)
		{
			Mesh* mesh = gameObject->getMesh();
			m_CommandBuffer->bindIndexBuffer(i, mesh->getIndexBuffer());
			m_CommandBuffer->bindDescriptorSets(i, m_Pipeline, { m_DescriptorSetPerFrame, gameObject->getDescriptorSet() });
			m_CommandBuffer->drawIndexed(i, mesh->getIndexBuffer());
		}
	}
}

GameObjectSimple* RendererSimple::createGameObject(Mesh* mesh, TextureVK* texture, SamplerVK* sampler)
{
	UniformBufferVK* uniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(glm::mat4));

	DescriptorSetVK* descriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, m_LayoutObject);
	descriptorSet->addStorageBuffer(BINDING_POS, mesh->getStorageBufferPos(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_NOR, mesh->getStorageBufferNor(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_UVS, mesh->getStorageBufferUV(), VK_WHOLE_SIZE, 0);
	descriptorSet->addUniformBuffer(BINDING_UBO, uniformBuffer);
	descriptorSet->addTexture(BINDING_TEX, texture, sampler);
	descriptorSet->submit();

	return new GameObjectSimple(descriptorSet, mesh, uniformBuffer, texture, sampler);
}

#include <glm/gtc/matrix_transform.hpp>

void RendererSimple::test(CameraVK* camera)
{
	UniformBufferObject ubo = {};

	ubo.view = camera->getView();
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->getExtent().width / (float)m_SwapChain->getExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	m_UniformBuffer->setData(&ubo);
}
