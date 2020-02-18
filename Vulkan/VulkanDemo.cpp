#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "VulkanDemo.h"

#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "TextureVK.h"
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

void VulkanDemo::preInit()
{
	m_Light = new LightVK(
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f));

	m_Camera = new CameraVK();
}

void VulkanDemo::onSwapChainCreated()
{
	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSkyBox = new RendererSkyBox(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();
	m_RendererSkyBox->init();

	m_RendererSimple->addLight(m_Light);

	m_GameObjectGround		= m_RendererSimple->createGameObject(RES::MESH_PLANE,	RES::TEXTURE_GROUND,	RES::SAMPLER_DEFAULT);
	m_GameObjectScreen		= m_RendererSimple->createGameObject(RES::MESH_PLANE,	RES::TEXTURE_ANIMATED,	RES::SAMPLER_DEFAULT);
	m_GameObjectFloor		= m_RendererSimple->createGameObject(RES::MESH_PLANE,	RES::TEXTURE_FLOOR,		RES::SAMPLER_DEFAULT);
	m_GameObjectSofa		= m_RendererSimple->createGameObject(RES::MESH_SOFA,	RES::TEXTURE_SOFA,		RES::SAMPLER_DEFAULT);
	m_GameObjectRightWall	= m_RendererSimple->createGameObject(RES::MESH_WALL2,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectFrontWall	= m_RendererSimple->createGameObject(RES::MESH_WALL2,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectWindow		= m_RendererSimple->createGameObject(RES::MESH_WINDOW,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectTv			= m_RendererSimple->createGameObject(RES::MESH_TV,		RES::TEXTURE_TV,		RES::SAMPLER_DEFAULT);
	m_GameObjectTable		= m_RendererSimple->createGameObject(RES::MESH_TABLE,	RES::TEXTURE_TABLE,		RES::SAMPLER_DEFAULT);
	m_GameObjectSkyBox		= m_RendererSkyBox->createGameObject(RES::MESH_CUBE,	RES::TEXTURE_SKYBOX,	RES::SAMPLER_DEFAULT);

	m_GameObjectScreen->scale(glm::vec3(3.0f, 1.555f, 1.0f));
	m_GameObjectScreen->translate(glm::vec3(0.0f, 0.895f, 3.3f));
	m_GameObjectScreen->rotate(-3.14 / 2.0, glm::vec3(1.0f, 0.0f, 0.0f));
	m_GameObjectScreen->rotate(3.14, glm::vec3(0.0f, 1.0f, 0.0f));
	m_GameObjectScreen->applyTransform();

	m_GameObjectFloor->scale(7);
	m_GameObjectFloor->applyTransform();

	m_GameObjectGround->translate(glm::vec3(0.0f, -0.1f, 0.0f));
	m_GameObjectGround->scale(100);
	m_GameObjectGround->applyTransform();

	m_GameObjectRightWall->rotate(-3.14 / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	m_GameObjectRightWall->translate(glm::vec3(0.0f, 0.0f, 3.0f));
	m_GameObjectRightWall->applyTransform();

	m_GameObjectFrontWall->translate(glm::vec3(0.0f, 0.0f, 3.0f));
	m_GameObjectFrontWall->applyTransform();

	m_GameObjectWindow->rotate(3.14 / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	m_GameObjectWindow->translate(glm::vec3(0.0f, 0.0f, 3.0f));
	m_GameObjectWindow->applyTransform();

	m_GameObjectTv->rotate(3.14, glm::vec3(0.0f, 1.0f, 0.0f));
	m_GameObjectTv->translate(glm::vec3(0.0f, 0.5f, -3.3f));
	m_GameObjectTv->applyTransform();

	m_GameObjectTable->translate(glm::vec3(0.0f, 0.0f, 1.0f));
	m_GameObjectTable->applyTransform();

	m_SimpleGameObjects.push_back(m_GameObjectScreen);
	m_SimpleGameObjects.push_back(m_GameObjectSofa);
	m_SimpleGameObjects.push_back(m_GameObjectRightWall);
	m_SimpleGameObjects.push_back(m_GameObjectFrontWall);
	m_SimpleGameObjects.push_back(m_GameObjectWindow);
	m_SimpleGameObjects.push_back(m_GameObjectTv);
	m_SimpleGameObjects.push_back(m_GameObjectTable);
	m_SimpleGameObjects.push_back(m_GameObjectFloor);
	m_SimpleGameObjects.push_back(m_GameObjectGround);

	std::vector<CommandBufferVK*> buffers;

	m_CommandBufferSimple = new CommandBufferVK(m_Device, m_SwapChain, false);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBufferSimple->begin(i, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, m_RenderPass);
	}
	m_RendererSimple->render(m_CommandBufferSimple, m_SimpleGameObjects);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBufferSimple->end(i);
	}
	buffers.push_back(m_CommandBufferSimple);


	m_CommandBufferSkyBox = new CommandBufferVK(m_Device, m_SwapChain, false);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBufferSkyBox->begin(i, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, m_RenderPass);
	}
	m_RendererSkyBox->render(m_CommandBufferSkyBox, { m_GameObjectSkyBox });
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBufferSkyBox->end(i);
	}
	buffers.push_back(m_CommandBufferSkyBox);

	m_CommandBufferPrimary = new CommandBufferVK(m_Device, m_SwapChain, true);
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBufferPrimary->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBufferPrimary->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBufferPrimary->writeSecondaryBuffers(i, buffers);
		m_CommandBufferPrimary->endRenderPass(i);
		m_CommandBufferPrimary->end(i);
	}
}

void VulkanDemo::onSwapChainReleased()
{
	for (GameObject* gameObject : m_SimpleGameObjects)
		delete gameObject;

	m_SimpleGameObjects.clear();

	delete m_CommandBufferPrimary;
	delete m_CommandBufferSkyBox;
	delete m_CommandBufferSimple;

	delete m_RendererSimple;
	delete m_RendererSkyBox;

	delete m_GameObjectSkyBox;
}

void VulkanDemo::init()
{
	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);

	RES::TEXTURE_ANIMATED->play();
}

void VulkanDemo::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);

	m_RendererSimple->update(deltaSeconds, m_Camera);
	m_RendererSkyBox->update(deltaSeconds, m_Camera);

	RES::TEXTURE_ANIMATED->submit();
}

CommandBufferVK* VulkanDemo::frame()
{
	return m_CommandBufferPrimary;
}

void VulkanDemo::shutdown()
{
	InputVK::removeKeyListener(this);
	InputVK::removeMouseListener(this);

	delete m_Camera;
	delete m_Light;
}

void VulkanDemo::onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE)
		InputVK::setCursorEnabled(true);
	else if (key == GLFW_KEY_SPACE)
		if (RES::TEXTURE_ANIMATED->isPlaying())
			RES::TEXTURE_ANIMATED->stop();
		else
			RES::TEXTURE_ANIMATED->play();
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
