#include "RendererSkyBox.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderVK.h"
#include "DescriptorSetLayoutVK.h"
#include "DescriptorSetVK.h"
#include "UniformBufferVK.H"
#include "GameObjectSkyBox.h"
#include "SamplerVK.h"
#include "SwapChainVK.h"
#include "CommandBufferVK.h"
#include "Mesh.h"
#include "CameraVK.h"
#include "TextureSkyBox.h"
#include "PipelineVK.h"

#define BINDING_POS 0
#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_UBO 3
#define BINDING_TEX 4

RendererSkyBox::RendererSkyBox(DeviceVK * device, SwapChainVK* swapChain, RenderPassVK* renderPass) :
	IRenderer(device, swapChain, renderPass)
{
}

RendererSkyBox::~RendererSkyBox()
{
	delete m_ShaderVertex;
	delete m_ShaderFragment;
	delete m_LayoutObject;
	delete m_Pipeline;
}

void RendererSkyBox::init()
{
	m_ShaderVertex = new ShaderVK(m_Device, "shaders/VertexShaderSky.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_ShaderVertex->compile();

	m_ShaderFragment = new ShaderVK(m_Device, "shaders/FragmentShaderSky.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_ShaderFragment->compile();

	m_LayoutObject = new DescriptorSetLayoutVK(m_Device);
	m_LayoutObject->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_NOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_UVS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addUniformBuffer(BINDING_UBO, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addTexture(BINDING_TEX, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_LayoutObject->submit();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_LayoutObject);
	m_Pipeline->addShader(m_ShaderVertex);
	m_Pipeline->addShader(m_ShaderFragment);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(UniformBufferObject));
	UniformBufferObject ubo = { glm::mat4(1), glm::mat4(1), glm::mat4(1) };
	m_UniformBuffer->setData(&ubo);
}

void RendererSkyBox::render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects)
{
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->bindPipeline(i, m_Pipeline);
		for (GameObject* gameObject : gameObjects)
		{
			Mesh* mesh = gameObject->getMesh();
			m_CommandBuffer->bindIndexBuffer(i, mesh->getIndexBuffer());
			m_CommandBuffer->bindDescriptorSets(i, m_Pipeline, { gameObject->getDescriptorSet() });
			m_CommandBuffer->drawIndexed(i, mesh->getIndexBuffer());
		}
	}
}

GameObjectSkyBox* RendererSkyBox::createGameObject(Mesh* mesh, TextureSkyBox* texture, SamplerVK* sampler)
{
	DescriptorSetVK* descriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, m_LayoutObject);
	descriptorSet->addStorageBuffer(BINDING_POS, mesh->getStorageBufferPos(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_NOR, mesh->getStorageBufferNor(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_UVS, mesh->getStorageBufferUV(), VK_WHOLE_SIZE, 0);
	descriptorSet->addUniformBuffer(BINDING_UBO, m_UniformBuffer);
	descriptorSet->addTexture(BINDING_TEX, texture, sampler);
	descriptorSet->submit();

	GameObjectSkyBox* gameObject = new GameObjectSkyBox(descriptorSet, mesh, m_UniformBuffer, texture);

	return gameObject;
}

void RendererSkyBox::update(float deltaSeconds, CameraVK* camera)
{
	UniformBufferObject ubo = {};

	ubo.camPos = glm::vec4(camera->getPosition(), 1.0F);
	ubo.view = glm::mat4(glm::mat3(camera->getView()));
	ubo.proj = glm::perspective(glm::radians(75.0f), m_SwapChain->getExtent().width / (float)m_SwapChain->getExtent().height, 0.01f, 500.0f);
	ubo.proj[1][1] *= -1;
	ubo.model = glm::scale(glm::mat4(1), glm::vec3(10, 10, 10));

	m_UniformBuffer->setData(&ubo);
}
