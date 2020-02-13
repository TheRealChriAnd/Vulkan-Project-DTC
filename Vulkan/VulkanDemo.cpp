#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

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
#include "RendererSkyBox.h"
#include "GameObjectSimple.h"
#include "GameObjectSkyBox.h"
#include "RES.h"
#include "InputVK.h"
#include "TextureSkyBox.h"

struct UniformBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

void VulkanDemo::init()
{
	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);

	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSkyBox = new RendererSkyBox(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();
	m_RendererSkyBox->init();

	m_Light = new LightVK(
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f));

	m_RendererSimple->addLight(m_Light);

	m_GameObjectGround = m_RendererSimple->createGameObject(RES::MESH_PLANE, RES::TEXTURE_FATBOY, RES::SAMPLER_DEFAULT);
	m_GameObject = m_RendererSimple->createGameObject(RES::MESH_TEST, RES::TEXTURE_TEST, RES::SAMPLER_DEFAULT);

	m_GameObjectSkyBox = m_RendererSkyBox->createGameObject(RES::MESH_CUBE, RES::TEXTURE_SKYBOX, RES::SAMPLER_DEFAULT);

	m_GameObjectGround->scale(10);
	m_GameObjectGround->applyTransform();

	m_SimpleGameObjects.push_back(m_GameObject);
	m_SimpleGameObjects.push_back(m_GameObjectGround);

	CommandBufferVK* m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
	}
	m_RendererSimple->render(m_CommandBuffer, m_SimpleGameObjects);
	m_RendererSkyBox->render(m_CommandBuffer, { m_GameObjectSkyBox });
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

	m_GameObject->rotate(deltaSeconds, glm::vec3(0.0F, 1.0F, 0.0F));
	m_GameObject->applyTransform();

	m_RendererSimple->update(deltaSeconds, m_Camera);
	m_RendererSkyBox->update(deltaSeconds, m_Camera);
}

const std::vector<CommandBufferVK*>& VulkanDemo::frame()
{
	return m_CommandBuffers;
}

void VulkanDemo::shutdown()
{
	InputVK::removeKeyListener(this);
	InputVK::removeMouseListener(this);

	for (CommandBufferVK* buffer : m_CommandBuffers)
		delete buffer;

	for (GameObject* gameObject : m_SimpleGameObjects)
		delete gameObject;

	delete m_Camera;
	delete m_Light;
	delete m_RendererSimple;

	delete m_GameObjectSkyBox;
	delete m_RendererSkyBox;
}

void VulkanDemo::onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		InputVK::setCursorEnabled(true);
	}
}

void VulkanDemo::onKeyReleased(int key)
{

}

void VulkanDemo::onMouseButtonPressed(int button)
{
	InputVK::setCursorEnabled(false);
}

void VulkanDemo::onMouseButtonRelease(int button)
{
}

void VulkanDemo::onMouseMove(const glm::vec2& pos, const glm::vec2& offset)
{
}
