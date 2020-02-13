#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VulkanDemo.h"

#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "TextureVK.h"
#include "SkyBoxVK.h"
#include "SamplerVK.h"
#include "CommandBufferVK.h"
#include "CameraVK.h"
#include "LightVK.h"
#include "GameObject.h"
#include "Mesh.h"
#include "RendererSimple.h"
#include "GameObjectSimple.h"

struct UniformBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

void VulkanDemo::init()
{
	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();

	m_Texture = new TextureVK(m_Device, 1);
	m_Texture->loadFromFile("textures/fatboy.png");

	m_Sampler = new SamplerVK(m_Device);

	m_Mesh = Mesh::createPlane(m_Device);

	m_Light = new LightVK(
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f));

	m_RendererSimple->addLight(m_Light);

	m_GameObject = m_RendererSimple->createGameObject(m_Mesh, m_Texture, m_Sampler);

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

	m_RendererSimple->update(deltaSeconds, m_Camera);
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
	delete m_Texture;
	delete m_Mesh;
	delete m_Light;
	delete m_RendererSimple;
	delete m_Sampler;
	delete m_GameObject;
}