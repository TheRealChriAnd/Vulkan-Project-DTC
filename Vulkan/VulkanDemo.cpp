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

std::vector<glm::vec4>  cubePos = {
	
	{-1.0f,-1.0f,-1.0f, 1.0f},  // -X side
	{-1.0f,-1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f,-1.0f, 1.0f},
	{-1.0f,-1.0f,-1.0f, 1.0f},
					   	   	 
	{-1.0f,-1.0f,-1.0f, 1.0f},  // -Z side
	{ 1.0f, 1.0f,-1.0f, 1.0f},
	{ 1.0f,-1.0f,-1.0f, 1.0f},
	{-1.0f,-1.0f,-1.0f, 1.0f},
	{-1.0f, 1.0f,-1.0f, 1.0f},
	{ 1.0f, 1.0f,-1.0f, 1.0f},
					   	   	 
	{-1.0f,-1.0f,-1.0f, 1.0f},  // -Y side
	{ 1.0f,-1.0f,-1.0f, 1.0f},
	{ 1.0f,-1.0f, 1.0f, 1.0f},
	{-1.0f,-1.0f,-1.0f, 1.0f},
	{ 1.0f,-1.0f, 1.0f, 1.0f},
	{-1.0f,-1.0f, 1.0f, 1.0f},
					   	   	 
	{-1.0f, 1.0f,-1.0f, 1.0f},  // +Y side
	{-1.0f, 1.0f, 1.0f, 1.0f},
	{ 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f,-1.0f, 1.0f},
	{ 1.0f, 1.0f, 1.0f, 1.0f},
	{ 1.0f, 1.0f,-1.0f, 1.0f},
					   	   	 
	{ 1.0f, 1.0f,-1.0f, 1.0f},  // +X side
	{ 1.0f, 1.0f, 1.0f, 1.0f},
	{ 1.0f,-1.0f, 1.0f, 1.0f},
	{ 1.0f,-1.0f, 1.0f, 1.0f},
	{ 1.0f,-1.0f,-1.0f, 1.0f},
	{ 1.0f, 1.0f,-1.0f, 1.0f},
					   	   	 
	{-1.0f, 1.0f, 1.0f, 1.0f},  // +Z side
	{-1.0f,-1.0f, 1.0f, 1.0f},
	{ 1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f,-1.0f, 1.0f, 1.0f},
	{ 1.0f,-1.0f, 1.0f, 1.0f},
	{ 1.0f, 1.0f, 1.0f, 1.0f}
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

std::vector<glm::vec2> cubeUv = {
	
	{0.0f, 1.0f},  // -X side
	{1.0f, 1.0f},
	{1.0f, 0.0f},
	{1.0f, 0.0f},
	{0.0f, 0.0f},
	{0.0f, 1.0f},
			   
	{1.0f, 1.0f},  // -Z side
	{0.0f, 0.0f},
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 0.0f},
			   
	{1.0f, 0.0f},  // -Y side
	{1.0f, 1.0f},
	{0.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 1.0f},
	{0.0f, 0.0f},
		   
	{1.0f, 0.0f},  // +Y side
	{0.0f, 0.0f},
	{0.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 1.0f},
	{1.0f, 1.0f},
			   
	{1.0f, 0.0f},  // +X side
	{0.0f, 0.0f},
	{0.0f, 1.0f},
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f},
			   
	{0.0f, 0.0f},  // +Z side
	{0.0f, 1.0f},
	{1.0f, 0.0f},
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{1.0f, 0.0f}
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
	//Rectangles
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

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(UniformBufferObject));

	m_DescriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout);
	m_DescriptorSet->addStorageBuffer(BINDING_POS, m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addStorageBuffer(BINDING_COLOR, m_StorageBufferNor, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addStorageBuffer(BINDING_UV, m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addUniformBuffer(BINDING_UNI, m_UniformBuffer);
	m_DescriptorSet->addTexture(BINDING_TEXTURE, m_Texture, m_Sampler);
	m_DescriptorSet->submit();

	//SkyBox
	m_VertexShader1 = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_VertexShader1->compile();

	m_FragmentShader1 = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_FragmentShader1->compile();

	m_DescriptorSetLayout1 = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout1->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout1->addStorageBuffer(BINDING_COLOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout1->addStorageBuffer(BINDING_UV, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout1->addUniformBuffer(BINDING_UNI, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout1->addTexture(BINDING_SKYBOX, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout1->submit();

	m_Pipeline1 = new PipelineVK(m_Device);
	m_Pipeline1->addDescriptorLayout(m_DescriptorSetLayout1);
	m_Pipeline1->addShader(m_VertexShader1);
	m_Pipeline1->addShader(m_FragmentShader1);
	m_Pipeline1->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	m_Skybox = new SkyBoxVK(m_Device);
	m_Skybox->loadFromFile("textures/clouds/west.bmp", 0);
	m_Skybox->loadFromFile("textures/clouds/east.bmp", 1);
	m_Skybox->loadFromFile("textures/clouds/down.bmp", 2);
	m_Skybox->loadFromFile("textures/clouds/up.bmp", 3);
	m_Skybox->loadFromFile("textures/clouds/south.bmp", 4);
	m_Skybox->loadFromFile("textures/clouds/north.bmp", 5);

	m_SkySampler = new SamplerVK(m_Device);

	m_StorageBufferPos1 = new StorageBufferVK(m_Device);
	m_StorageBufferPos1->setData(cubePos.data(), sizeof(glm::vec4) * cubePos.size(), 0);
					  
	m_StorageBufferNor1 = new StorageBufferVK(m_Device);
	m_StorageBufferNor1->setData(cubeColor.data(), sizeof(glm::vec4) * cubeColor.size(), 0);

	m_StorageBufferUV1 = new StorageBufferVK(m_Device);
	m_StorageBufferUV1->setData(cubeUv.data(), sizeof(glm::vec2) * cubeUv.size(), 0);

	m_IndexBuffer1 = new IndexBufferVK(m_Device, cubeIndices);

	m_UniformBuffer1 = new UniformBufferVK(m_Device, m_SwapChain);
	m_UniformBuffer1->setBufferSize(sizeof(UniformBufferObject));

	m_DescriptorSet1 = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout1);
	m_DescriptorSet1->addStorageBuffer(BINDING_POS, m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	m_DescriptorSet1->addStorageBuffer(BINDING_COLOR, m_StorageBufferNor, VK_WHOLE_SIZE, 0);
	m_DescriptorSet1->addStorageBuffer(BINDING_UV, m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	m_DescriptorSet1->addUniformBuffer(BINDING_UNI, m_UniformBuffer);
	m_DescriptorSet1->addSkyBoxTexture(BINDING_SKYBOX, m_Skybox, m_SkySampler);
	m_DescriptorSet1->submit();


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

	//recording SkyBox
	CommandBufferVK* m_CommandBuffer1 = new CommandBufferVK(m_Device, m_SwapChain);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBuffer->bindPipeline(i, m_Pipeline1);
		m_CommandBuffer->bindIndexBuffer(i, m_IndexBuffer1);
		m_CommandBuffer->bindDescriptorSets(i, m_Pipeline1, { m_DescriptorSet1 });
		m_CommandBuffer->drawIndexed(i, m_IndexBuffer1);
		m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);
	}

	m_CommandBuffers.push_back(m_CommandBuffer);
	m_CommandBuffers.push_back(m_CommandBuffer1);

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
