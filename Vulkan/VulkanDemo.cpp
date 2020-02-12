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
#include "LightVK.h"
#include "GameObject.h"
#include "Mesh.h"
#include "RendererSimple.h"
#include "GameObjectSimple.h"

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4
#define BINDING_SKYBOX 5
#define BINDING_LIGHT 6

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
	m_Texture = new TextureVK(m_Device, 1);
	m_Texture->loadFromFile("textures/fatboy.png");

	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();

	m_Mesh = Mesh::createPlane(m_Device);
	m_GameObject = m_RendererSimple->createGameObject(m_Mesh, m_Texture, new SamplerVK(m_Device));

	m_Light = new LightVK(
		glm::vec3(-0.2f, -1.0f, -0.3f), 
		glm::vec3(0.2f, 0.2f, 0.2f), 
		glm::vec3(0.5f, 0.5f, 0.5f), 
		glm::vec3(1.0f, 1.0f, 1.0f));


	CommandBufferVK* m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
	}
	m_RendererSimple->render(m_CommandBuffer, { m_GameObject });
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);
	}

	m_CommandBuffers.push_back(m_CommandBuffer);

	m_Camera = new CameraVK();
}

void VulkanDemo::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);

	m_GameObject->rotate(deltaSeconds, glm::vec3(0, 0, 1.0F));
	m_GameObject->applyTransform();

	m_RendererSimple->test(m_Camera);
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
	delete m_Sampler;
	delete m_Light;
	delete m_IndexBuffer;
	delete m_DescriptorSetLayout;
	delete m_DescriptorSetLayout2;
	delete m_DescriptorSet2;
	delete m_Pipeline;
	delete m_Mesh;
	delete m_GameObject;
}
