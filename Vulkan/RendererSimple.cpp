#include <glm/gtc/matrix_transform.hpp>
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
#include "LightVK.h"

#define BINDING_G_POS 0
#define BINDING_G_NOR 1
#define BINDING_G_UVS 2
#define BINDING_G_UBO 3
#define BINDING_G_TEX 4

#define BINDING_G_UCL 0

#define BINDING_R_AS 0
#define BINDING_R_RI 1
#define BINDING_R_UBO 2

RendererSimple::RendererSimple(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass) : IRenderer(device, swapChain, renderPass)
{

}

RendererSimple::~RendererSimple()
{
	delete m_Pipeline;
	delete m_DescriptorSetPerFrame;
	delete m_LayoutObject;
	delete m_LayoutCamera;
	delete m_UniformBuffer;
	delete m_ShaderVertex;
	delete m_ShaderFragment;
}

void RendererSimple::init()
{
	// __GRAPHICS_PIPELINE__
	m_LayoutCamera = new DescriptorSetLayoutVK(m_Device);
	m_LayoutCamera->addUniformBuffer(BINDING_G_UCL, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	m_LayoutCamera->submit();

	m_LayoutObject = new DescriptorSetLayoutVK(m_Device);
	m_LayoutObject->addStorageBuffer(BINDING_G_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_G_NOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addStorageBuffer(BINDING_G_UVS, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addUniformBuffer(BINDING_G_UBO, VK_SHADER_STAGE_VERTEX_BIT);
	m_LayoutObject->addTexture(BINDING_G_TEX, VK_SHADER_STAGE_FRAGMENT_BIT);

	m_LayoutObject->submit();

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(UniformBufferObject));

	m_DescriptorSetPerFrame = new DescriptorSetVK(m_Device, m_SwapChain, m_LayoutCamera);
	m_DescriptorSetPerFrame->addUniformBuffer(BINDING_G_UCL, m_UniformBuffer);
	m_DescriptorSetPerFrame->submit();

	m_ShaderVertex = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_ShaderVertex->compile();

	m_ShaderFragment = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_ShaderFragment->compile();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_LayoutCamera);
	m_Pipeline->addDescriptorLayout(m_LayoutObject);
	m_Pipeline->addShader(m_ShaderVertex);
	m_Pipeline->addShader(m_ShaderFragment);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	// __RAY_TRACING_PIPELINE__

	m_LayoutRayTracing = new DescriptorSetLayoutVK(m_Device);
	m_LayoutRayTracing->addAccelerationStructure(BINDING_R_AS, VK_SHADER_STAGE_RAYGEN_BIT_NV);
	m_LayoutRayTracing->addAccelerationStructure(BINDING_R_RI, VK_SHADER_STAGE_RAYGEN_BIT_NV);
	m_LayoutRayTracing->addAccelerationStructure(BINDING_R_UBO, VK_SHADER_STAGE_RAYGEN_BIT_NV);

	m_LayoutRayTracing->submit();

	m_ShaderRaygen = new ShaderVK(m_Device, "shaders/Raygen.rgen", VK_SHADER_STAGE_RAYGEN_BIT_NV);
	m_ShaderRaygen->compile();

	m_ShaderMiss = new ShaderVK(m_Device, "shaders/Miss.rmiss", VK_SHADER_STAGE_MISS_BIT_NV);
	m_ShaderMiss->compile();

	m_ShaderChit = new ShaderVK(m_Device, "shaders/Rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV);
	m_ShaderChit->compile();
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
	descriptorSet->addStorageBuffer(BINDING_G_POS, mesh->getStorageBufferPos(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_G_NOR, mesh->getStorageBufferNor(), VK_WHOLE_SIZE, 0);
	descriptorSet->addStorageBuffer(BINDING_G_UVS, mesh->getStorageBufferUV(), VK_WHOLE_SIZE, 0);
	descriptorSet->addUniformBuffer(BINDING_G_UBO, uniformBuffer);
	descriptorSet->addTexture(BINDING_G_TEX, texture, sampler);
	descriptorSet->submit();

	GameObjectSimple* gameObject = new GameObjectSimple(descriptorSet, mesh, uniformBuffer, texture, sampler);
	gameObject->applyTransform();

	return gameObject;
}

void RendererSimple::update(float deltaSeconds, CameraVK* camera)
{
	UniformBufferObject ubo = {};

	ubo.camPos = glm::vec4(camera->getPosition(), 1.0F);
	ubo.view = camera->getView();
	ubo.proj = glm::perspective(glm::radians(75.0f), m_SwapChain->getExtent().width / (float)m_SwapChain->getExtent().height, 0.01f, 500.0f);
	ubo.proj[1][1] *= -1;

	if (!m_Lights.empty())
	{
		LightVK* light = *m_Lights.begin();
		ubo.dir = glm::vec4(light->getDir(), 1.0F);
		ubo.ambient = glm::vec4(light->getAmbient(), 1.0F);
		ubo.diffuse = glm::vec4(light->getDiffuse(), 1.0F);
		ubo.specular = glm::vec4(light->getSpecular(), 1.0F);
	}

	m_UniformBuffer->setData(&ubo);
}
