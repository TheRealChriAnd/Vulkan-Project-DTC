#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <atomic>

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
#include "GameObjectAnimated.h"
#include "RES.h"
#include "InputVK.h"
#include "LightPoint.h"
#include "TextureSkyBox.h"
#include "ThreadManager.h"
#include "CommandPoolVK.h"
#include "Profiler.h"


void VulkanDemo::preInit()
{
	m_PointLight.push_back(new LightPoint(glm::vec3(1.5f, 2.1f, 3.45f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(-1.5f, 2.1f, 3.45f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(1.5f, 0.7f, 3.45f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(-1.5f, 0.7f, 3.45f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	
	m_PointLight.push_back(new LightPoint(glm::vec3(-0.35f, 2.1f, 3.45f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(0.35f, 2.1f, 3.45f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(-0.35f, 0.7f, 3.45f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(0.35f, 0.7f, 3.45f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	
	m_PointLight.push_back(new LightPoint(glm::vec3(1.5f, 1.4f, 3.45f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));
	m_PointLight.push_back(new LightPoint(glm::vec3(-1.5f, 1.4f, 3.45f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.9f, 5.8f)));

	
	m_PointLight.push_back(new LightPoint(glm::vec3(0.0f, 1.4f, 3.45f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.09f, 0.032f)));

	m_Camera = new CameraVK();

	m_Camera->addNode(glm::vec3(0.0f, 1.7f, -2.0f), glm::vec3(0.0f, 1.5f, 3.3f));
	m_Camera->addNode(glm::vec3(0.0f, 1.7f, 2.0f), glm::vec3(0.0f, 1.5f, 3.3f));
	m_Camera->addNode(glm::vec3(3.0f, 1.7f, 0.0f), glm::vec3(0.0f, 1.5f, 3.3f));
	m_Camera->addNode(glm::vec3(0.0f, 3.5f, -3.0f), glm::vec3(0.0f, 1.5f, 3.3f));
	m_Camera->addNode(glm::vec3(-3.0f, 1.7f, 0.0f), glm::vec3(0.0f, 1.5f, 3.3f));

	m_Camera->startFollowPath(true);
	m_Camera->addListener(this);
	m_BenchmarkRunning = false;

	m_GraphicsCommandPool2 = new CommandPoolVK(m_Device, false);
}

void VulkanDemo::onSwapChainCreated()
{
	m_RendererSimple = new RendererSimple(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSkyBox = new RendererSkyBox(m_Device, m_SwapChain, m_RenderPass);
	m_RendererSimple->init();
	m_RendererSkyBox->init();

	for (int i = 0; i < m_PointLight.size(); i++)
	{
		m_RendererSimple->addLight(m_PointLight[i]);
	}

	std::vector<GameObjectSimple*> m_ScreenGameObjects;

	for (int i = 0; i < ANIMATED_TEXTURES; i++)
	//for (int i = 0; i < 1; i++)
		m_ScreenGameObjects.push_back(m_RendererSimple->createGameObjectAnimatedTexture(RES::MESH_PLANE, RES::getAnimatedTexture(i), RES::SAMPLER_DEFAULT));

	m_GameObjectGround		= m_RendererSimple->createGameObject(RES::MESH_PLANE,	RES::TEXTURE_GROUND,	RES::SAMPLER_DEFAULT);
	m_GameObjectFloor		= m_RendererSimple->createGameObject(RES::MESH_PLANE,	RES::TEXTURE_FLOOR,		RES::SAMPLER_DEFAULT);
	m_GameObjectSofa		= m_RendererSimple->createGameObject(RES::MESH_SOFA,	RES::TEXTURE_SOFA,		RES::SAMPLER_DEFAULT);
	m_GameObjectRightWall	= m_RendererSimple->createGameObject(RES::MESH_WALL2,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectFrontWall	= m_RendererSimple->createGameObject(RES::MESH_WALL2,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectWindow		= m_RendererSimple->createGameObject(RES::MESH_WINDOW,	RES::TEXTURE_THIN,		RES::SAMPLER_DEFAULT);
	m_GameObjectTv			= m_RendererSimple->createGameObject(RES::MESH_TV,		RES::TEXTURE_TV,		RES::SAMPLER_DEFAULT);
	m_GameObjectTable		= m_RendererSimple->createGameObject(RES::MESH_TABLE,	RES::TEXTURE_TABLE,		RES::SAMPLER_DEFAULT);
	m_GameObjectSkyBox		= m_RendererSkyBox->createGameObject(RES::MESH_CUBE,	RES::TEXTURE_SKYBOX,	RES::SAMPLER_DEFAULT);

	m_ScreenGameObjects[0]->scale(glm::vec3(3.0f, 1.555f, 1.0f));
	m_ScreenGameObjects[0]->translate(glm::vec3(0.0f, 0.895f, 3.3f));
	m_ScreenGameObjects[0]->rotate(-3.14 / 2.0, glm::vec3(1.0f, 0.0f, 0.0f));
	m_ScreenGameObjects[0]->rotate(3.14, glm::vec3(0.0f, 1.0f, 0.0f));
	m_ScreenGameObjects[0]->applyTransform();

	for (int i = 1; i < m_ScreenGameObjects.size(); i++)
	{
		GameObjectSimple* gameObject = m_ScreenGameObjects[i];
		gameObject->scale(glm::vec3(3.0f, 1.555f, 1.0f));
		gameObject->translate(glm::vec3(1.1f * i, 0.895f, 3.3f));
		gameObject->rotate(-3.14 / 2.0, glm::vec3(1.0f, 0.0f, 0.0f));
		gameObject->rotate(3.14, glm::vec3(0.0f, 1.0f, 0.0f));
		gameObject->applyTransform();
	}

	m_GameObjectFloor->scale(7);
	m_GameObjectFloor->applyTransform();

	m_GameObjectGround->translate(glm::vec3(0.0f, -0.1f, 0.0f));

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


	for (int i = 0; i < m_ScreenGameObjects.size(); i++)
		m_SimpleGameObjects.push_back(m_ScreenGameObjects[i]);

	m_SimpleGameObjects.push_back(m_GameObjectSofa);
	m_SimpleGameObjects.push_back(m_GameObjectRightWall);
	m_SimpleGameObjects.push_back(m_GameObjectFrontWall);
	m_SimpleGameObjects.push_back(m_GameObjectWindow);
	m_SimpleGameObjects.push_back(m_GameObjectTv);
	m_SimpleGameObjects.push_back(m_GameObjectTable);
	m_SimpleGameObjects.push_back(m_GameObjectFloor);
	m_SimpleGameObjects.push_back(m_GameObjectGround);


	m_CommandBufferSimple = new CommandBufferVK(m_Device, m_GraphicsCommandPool2, m_SwapChain, false);
	m_CommandBufferSkyBox = new CommandBufferVK(m_Device, m_Device->getGraphicsCommandPool(), m_SwapChain, false);
	m_CommandBufferPrimary = new CommandBufferVK(m_Device, m_Device->getGraphicsCommandPool(), m_SwapChain, true);
}

void VulkanDemo::init()
{
	InputVK::addKeyListener(this);
	InputVK::addMouseListener(this);
	
	RES::getAnimatedTexture(0)->setOnFrameReadyCallback(std::bind(&VulkanDemo::onTVFrameReady, this, std::placeholders::_1));
	RES::VIDEO_TV->play();
}

void VulkanDemo::createCommandBuffers(size_t index)
{
	std::vector<CommandBufferVK*> buffers;

	buffers.push_back(m_CommandBufferSkyBox);
	buffers.push_back(m_CommandBufferSimple);

	std::atomic_bool done = false;
	
#ifdef VULKAN
	ThreadManager::scheduleExecution([&, this]()
	{
		this->drawSimpleGameObjects(index, done);
	});
#else
	VulkanDemo::drawSimpleGameObjects(index, done);
#endif

	m_CommandBufferSkyBox->begin(index, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, m_RenderPass);
	m_RendererSkyBox->render(m_CommandBufferSkyBox, index, { m_GameObjectSkyBox });
	m_CommandBufferSkyBox->end(index);

#ifdef VULKAN
	while (!done) {};
#endif

	m_CommandBufferPrimary->begin(index, (VkCommandBufferUsageFlagBits)0);
	m_CommandBufferPrimary->beginRenderPass(index, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
	m_CommandBufferPrimary->writeSecondaryBuffers(index, buffers);
	m_CommandBufferPrimary->endRenderPass(index);
	m_CommandBufferPrimary->end(index);
}

void VulkanDemo::update(float deltaSeconds)
{
	m_Camera->update(deltaSeconds);

	m_RendererSimple->update(deltaSeconds, m_Camera);
	m_RendererSkyBox->update(deltaSeconds, m_Camera);

	for (int i = 0; i < ANIMATED_TEXTURES; i++)
		RES::getAnimatedTexture(i)->update(deltaSeconds);

	createCommandBuffers(m_SwapChain->getCurrentImageIndex());
}

CommandBufferVK* VulkanDemo::frame()
{
	return m_CommandBufferPrimary;
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

void VulkanDemo::shutdown()
{
	InputVK::removeKeyListener(this);
	InputVK::removeMouseListener(this);

	delete m_Camera;
	for (LightPoint* light : m_PointLight)
	{
		delete light;
	}
	
	delete m_GraphicsCommandPool2;
}

void VulkanDemo::onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE)
		InputVK::setCursorEnabled(true);
	else if (key == GLFW_KEY_SPACE)
		if (RES::VIDEO_TV->isPlaying())
			RES::VIDEO_TV->stop();
		else
			RES::VIDEO_TV->play();
	else if (key == GLFW_KEY_ENTER)
	{
		if (!m_BenchmarkRunning)
		{
			std::cout << "Benchmark started!" << std::endl;
			Profiler::reset();
			m_Camera->startFollowPath(true);
			m_BenchmarkRunning = true;
		}
		else
		{
			std::cout << "Benchmark cancelled!" << std::endl;
			m_BenchmarkRunning = false;
			Profiler::reset();
		}
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

void VulkanDemo::onCameraPathEnded(CameraVK* camera)
{
	if (m_BenchmarkRunning)
	{
		Profiler::printResults();
		m_BenchmarkRunning = false;
	}
}

void VulkanDemo::onTVFrameReady(TextureAnimated* texture)
{
	int width = texture->getWidth();
	int height = texture->getHeight();
	int squareSize = height * 0.05F;

	glm::vec4 sampleColor[10];
	sampleColor[0] = glm::vec4(texture->getSampleColor(squareSize, squareSize, 0, 0), 255.0F) / 255.0F;
	sampleColor[1] = glm::vec4(texture->getSampleColor(squareSize, squareSize, width - squareSize - 1, 0), 255.0F) / 255.0F;
	sampleColor[2] = glm::vec4(texture->getSampleColor(squareSize, squareSize, 0, height - squareSize - 1), 255.0F) / 255.0F;
	sampleColor[3] = glm::vec4(texture->getSampleColor(squareSize, squareSize, width - squareSize - 1, height - squareSize - 1), 255.0F) / 255.0F;

	sampleColor[4] = glm::vec4(texture->getSampleColor(squareSize, squareSize, 2 * width / 3 - squareSize / 2, 0), 255.0F) / 255.0F;
	sampleColor[5] = glm::vec4(texture->getSampleColor(squareSize, squareSize, width / 3 - squareSize / 2, 0), 255.0F) / 255.0F;

	sampleColor[6] = glm::vec4(texture->getSampleColor(squareSize, squareSize, 2 * width / 3 - squareSize / 2, height - squareSize - 1), 255.0F) / 255.0F;
	sampleColor[7] = glm::vec4(texture->getSampleColor(squareSize, squareSize, width / 3 - squareSize / 2, height - squareSize - 1), 255.0F) / 255.0F;

	sampleColor[8] = glm::vec4(texture->getSampleColor(squareSize, squareSize, 0, height / 2 - squareSize / 2), 255.0F) / 255.0F;
	sampleColor[9] = glm::vec4(texture->getSampleColor(squareSize, squareSize, width - squareSize - 1, height / 2 - squareSize / 2), 255.0F) / 255.0F;

	for (int i = 0; i < 10; i++)
	{
		m_PointLight[i]->setAmbientColor(sampleColor[i]);
		m_PointLight[i]->setDiffuseColor(sampleColor[i]);
		m_PointLight[i]->setSpecColor(sampleColor[i]);
	}
	glm::vec3 color = RES::getAnimatedTexture(0)->getSampleColor(width, height, 0, 0) / 255.0F;
	color *= 5.0f;
	float length = glm::sqrt(color.x * color.x + color.y * color.y + color.z * color.z);
	float maxValue = glm::sqrt(3);
	if (length > maxValue)
	{
		color.x /= length;
		color.y /= length;
		color.z /= length;

		color *= maxValue;
	}

	glm::vec4 finalColor(color, 1.0F);

	int index = m_PointLight.size() - 1;
	m_PointLight[index]->setAmbientColor(finalColor);
	m_PointLight[index]->setDiffuseColor(finalColor);
	m_PointLight[index]->setSpecColor(finalColor);

	Profiler::count("TV_FRAMES");
}

void VulkanDemo::drawSimpleGameObjects(size_t index, std::atomic_bool& done)
{
	m_CommandBufferSimple->begin(index, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, m_RenderPass);
	m_RendererSimple->render(m_CommandBufferSimple, index, m_SimpleGameObjects);
	m_CommandBufferSimple->end(index);
	done = true;
}
