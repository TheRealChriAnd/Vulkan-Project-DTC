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
#include "SkyBoxVK.h"
#include "SamplerVK.h"
#include "CommandBufferVK.h"
#include "IndexBufferVK.h"
#include "CameraVK.h"
#include "GameObject.h"
#include "MeshVK.h"

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4
#define BINDING_SKYBOX 5

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

std::vector<glm::vec4> cubeColor = {
	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f}
};

const std::vector<uint16_t> cubeIndices= {
		0, 1, 2, 0, 2, 3,    // front
		4, 5, 6, 4, 6, 7,    // back
		8, 9, 10, 8, 10, 11,   // top
		12, 13, 14, 12, 14, 15,   // bottom
		16, 17, 18, 16, 18, 19,   // right
		20, 21, 22, 20, 22, 23   // left
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
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

void VulkanDemo::init()
{
	//Rectangles
	m_VertexShader = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_VertexShader->compile();

	m_FragmentShader = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_FragmentShader->compile();

	m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addStorageBuffer(0, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addUniformBuffer(2, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addTexture(3, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->submit();

	m_DescriptorSetLayout2 = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout2->addUniformBuffer(0, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout2->submit();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_DescriptorSetLayout2);
	m_Pipeline->addDescriptorLayout(m_DescriptorSetLayout);
	m_Pipeline->addShader(m_VertexShader);
	m_Pipeline->addShader(m_FragmentShader);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	m_Texture = new TextureVK(m_Device, 1);
	m_Texture->loadFromFile("textures/fatboy.png");

	m_IndexBuffer = new IndexBufferVK(m_Device, indices);

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(UniformBufferObject));

	m_DescriptorSet2 = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout2);
	m_DescriptorSet2->addUniformBuffer(0, m_UniformBuffer);
	m_DescriptorSet2->submit();

	m_Mesh = MeshVK::createPlane(m_Device);
	m_GameObject = new GameObject(m_DescriptorSetLayout, m_Mesh, m_Texture);

	//SkyBox
	//m_VertexShader1 = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	//m_VertexShader1->compile();

	//m_FragmentShader1 = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	//m_FragmentShader1->compile();

	//m_DescriptorSetLayout1 = new DescriptorSetLayoutVK(m_Device);
	//m_DescriptorSetLayout1->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	//m_DescriptorSetLayout1->addStorageBuffer(BINDING_COLOR, VK_SHADER_STAGE_VERTEX_BIT);
	//m_DescriptorSetLayout1->addStorageBuffer(BINDING_UV, VK_SHADER_STAGE_VERTEX_BIT);
	//m_DescriptorSetLayout1->addUniformBuffer(BINDING_UNI, VK_SHADER_STAGE_VERTEX_BIT);
	//m_DescriptorSetLayout1->addTexture(BINDING_SKYBOX, VK_SHADER_STAGE_FRAGMENT_BIT);
	//m_DescriptorSetLayout1->submit();



	//m_Skybox = new SkyBoxVK(m_Device, m_SwapChain, m_RenderPass);
	//m_Skybox->loadFromFile("textures/clouds/west.bmp", 0);
	//m_Skybox->loadFromFile("textures/clouds/east.bmp", 1);
	//m_Skybox->loadFromFile("textures/clouds/down.bmp", 2);
	//m_Skybox->loadFromFile("textures/clouds/up.bmp", 3);
	//m_Skybox->loadFromFile("textures/clouds/south.bmp", 4);
	//m_Skybox->loadFromFile("textures/clouds/north.bmp", 5);

	

					  
	//m_StorageBufferCol1 = new StorageBufferVK(m_Device);
	//m_StorageBufferCol1->setData(cubeColor.data(), sizeof(glm::vec4) * cubeColor.size(), 0);

	//m_DescriptorSet1 = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout1);
	//m_DescriptorSet1->addStorageBuffer(BINDING_POS, m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet1->addStorageBuffer(BINDING_COLOR, m_StorageBufferNor, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet1->addStorageBuffer(BINDING_UV, m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet1->addUniformBuffer(BINDING_UNI, m_UniformBuffer);
	//m_DescriptorSet1->addSkyBoxTexture(BINDING_SKYBOX, m_Skybox, m_SkySampler);
	//m_DescriptorSet1->submit();


	CommandBufferVK* m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBuffer->bindPipeline(i, m_Pipeline);
		m_CommandBuffer->bindIndexBuffer(i, m_IndexBuffer);
		m_CommandBuffer->bindDescriptorSets(i, m_Pipeline, { m_DescriptorSet2, m_GameObject->m_DescriptorSet });
		m_CommandBuffer->drawIndexed(i, m_IndexBuffer);
		m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);
	}

	//recording SkyBox
	//CommandBufferVK* m_CommandBuffer1 = new CommandBufferVK(m_Device, m_SwapChain);
	//for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	//{
	//	m_CommandBuffer1->begin(i, (VkCommandBufferUsageFlagBits)0);
	//	m_CommandBuffer1->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
	//	m_CommandBuffer1->bindPipeline(i, m_Skybox->getPipeline());
	//	m_CommandBuffer1->bindIndexBuffer(i, m_IndexBuffer1);
	//	m_CommandBuffer1->bindDescriptorSets(i, m_Skybox->getPipeline(), { m_Skybox->getDescriptorSet() });
	//	m_CommandBuffer1->drawIndexed(i, m_IndexBuffer1);
	//	m_CommandBuffer1->endRenderPass(i);
	//	m_CommandBuffer1->end(i);
	//}

	m_CommandBuffers.push_back(m_CommandBuffer);
	//m_CommandBuffers.push_back(m_Skybox->getCommandBuffer());

	m_Camera = new CameraVK();
}

void VulkanDemo::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);

	UniformBufferObject ubo = {};
	m_GameObject->rotate(deltaSeconds, glm::vec3(0, 0, 1.0F));
	m_GameObject->applyTransform();

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
	delete m_UniformBuffer;
	delete m_Texture;
	delete m_IndexBuffer;
	delete m_DescriptorSetLayout;
	delete m_DescriptorSetLayout2;
	delete m_DescriptorSet2;
	delete m_Pipeline;
	delete m_Mesh;
	delete m_GameObject;
}
