#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "HelloRT.h"

#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "CommandBufferVK.h"
#include "CameraVK.h"
#include "GameObject.h"
#include "Mesh.h"
#include "RendererSimple.h"
#include "RendererSkyBox.h"
#include "GameObjectSimple.h"
#include "GameObjectSkyBox.h"
#include "RES.h"
#include "InputVK.h"
#include "TextureSkyBox.h"

HelloRT::HelloRT()
{
}

HelloRT::~HelloRT()
{
}

void HelloRT::preInit()
{
	m_Camera = new CameraVK();
}

void HelloRT::init()
{
	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);
}

void HelloRT::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);

	m_RendererSimple->update(deltaSeconds, m_Camera);
	m_RendererSkyBox->update(deltaSeconds, m_Camera);

	RES::TEXTURE_ANIMATED->submit();
}

const std::vector<CommandBufferVK*>& HelloRT::frame()
{
	return m_CommandBuffers;
}

void HelloRT::shutdown()
{
	InputVK::removeKeyListener(this);
	InputVK::removeMouseListener(this);

	delete m_Camera;
}

void HelloRT::onSwapChainReleased()
{
	for (CommandBufferVK* buffer : m_CommandBuffers)
		delete buffer;

	for (GameObject* gameObject : m_SimpleGameObjects)
		delete gameObject;

	m_CommandBuffers.clear();
	m_SimpleGameObjects.clear();

	delete m_RendererSimple;
	delete m_RendererSkyBox;

	delete m_GameObjectSkyBox;
}

void HelloRT::onSwapChainCreated()
{
	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSkyBox = new RendererSkyBox(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();
	m_RendererSkyBox->init();

	m_GameObjectSkyBox = m_RendererSkyBox->createGameObject(RES::MESH_CUBE, RES::TEXTURE_SKYBOX, RES::SAMPLER_DEFAULT);

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
}

void HelloRT::onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		InputVK::setCursorEnabled(true);
	}
}

void HelloRT::onKeyReleased(int key)
{
}

void HelloRT::onMouseButtonPressed(int button)
{
	InputVK::setCursorEnabled(false);
}

void HelloRT::onMouseButtonRelease(int button)
{
}

void HelloRT::onMouseMove(const glm::vec2& pos, const glm::vec2& offset)
{
}
