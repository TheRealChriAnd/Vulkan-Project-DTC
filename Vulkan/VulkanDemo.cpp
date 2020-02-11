#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VulkanDemo.h"

#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"

#include "DescriptorSetLayoutVK.h"
#include "PipelineVK.h"
#include "DescriptorSetVK.h"
#include "ShaderVK.h"
#include "StorageBufferVK.h"
#include "UniformBufferVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "CommandBufferVK.h"
#include "IndexBufferVK.h"
#include "CameraVK.h"

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4

std::vector<glm::vec4> pos = {
	{-0.5f, -0.5f, 0.0f, 1.0f},
	{0.5f, -0.5f, 0.0f, 1.0f},
	{0.5f, 0.5f, 0.0f, 1.0f},
	{-0.5f, 0.5f, 0.0f, 1.0f},

	{-0.5f, -0.5f, -0.5f, 1.0f},
	{0.5f, -0.5f, -0.5f, 1.0f},
	{0.5f, 0.5f, -0.5f, 1.0f},
	{-0.5f, 0.5f, -0.5f, 1.0f}
};

std::vector<glm::vec4> color = {
	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f}
};

std::vector<glm::vec2> uv = {
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f},

	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

void VulkanDemo::init()
{
	m_VertexShader = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_VertexShader->compile();

	m_FragmentShader = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_FragmentShader->compile();

	m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_COLOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_UV, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addUniformBuffer(BINDING_UNI, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addTexture(BINDING_TEXTURE, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->submit();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_DescriptorSetLayout);
	m_Pipeline->addShader(m_VertexShader);
	m_Pipeline->addShader(m_FragmentShader);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	m_Texture = new TextureVK(m_Device);
	m_Texture->loadFromFile("textures/fatboy.png");

	m_Sampler = new SamplerVK(m_Device);

	m_StorageBufferPos = new StorageBufferVK(m_Device);
	m_StorageBufferPos->setData(pos.data(), sizeof(glm::vec4) * pos.size(), 0);

	m_StorageBufferNor = new StorageBufferVK(m_Device);
	m_StorageBufferNor->setData(color.data(), sizeof(glm::vec4) * color.size(), 0);

	m_StorageBufferUV = new StorageBufferVK(m_Device);
	m_StorageBufferUV->setData(uv.data(), sizeof(glm::vec2) * uv.size(), 0);

	m_IndexBuffer = new IndexBufferVK(m_Device, indices);

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain);
	m_UniformBuffer->setBufferSize(sizeof(UniformBufferObject));

	m_DescriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout);
	m_DescriptorSet->addStorageBuffer(BINDING_POS, m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addStorageBuffer(BINDING_COLOR, m_StorageBufferNor, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addStorageBuffer(BINDING_UV, m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addUniformBuffer(BINDING_UNI, m_UniformBuffer);
	m_DescriptorSet->addTexture(BINDING_TEXTURE, m_Texture, m_Sampler);
	m_DescriptorSet->submit();

	CommandBufferVK* m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBuffer->bindPipeline(i, m_Pipeline);
		m_CommandBuffer->bindIndexBuffer(i, m_IndexBuffer);
		m_CommandBuffer->bindDescriptorSets(i, m_Pipeline, { m_DescriptorSet });
		m_CommandBuffer->drawIndexed(i, m_IndexBuffer);
		m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);
	}

	m_CommandBuffers.push_back(m_CommandBuffer);

	m_Camera = new CameraVK();
}

void VulkanDemo::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);
	m_Rotation += deltaSeconds;
	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation * 50), glm::vec3(0.0f, 0.0f, 1.0f));
	//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = m_Camera->getView();
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->getExtent().width / (float)m_SwapChain->getExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	m_UniformBuffer->setData(&ubo);
}

const std::vector<CommandBufferVK*>& VulkanDemo::frame()
{
	return m_CommandBuffers;
}

void VulkanDemo::shutdown()
{
	for (CommandBufferVK* buffer : m_CommandBuffers)
		delete buffer;

	delete m_Camera;
	delete m_VertexShader;
	delete m_FragmentShader;
	delete m_StorageBufferPos;
	delete m_StorageBufferNor;
	delete m_StorageBufferUV;
	delete m_UniformBuffer;
	delete m_Texture;
	delete m_Sampler;
	delete m_IndexBuffer;
	delete m_DescriptorSet;
	delete m_DescriptorSetLayout;
	delete m_Pipeline;
}
