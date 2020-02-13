#pragma once
#include "Application.h"
#include "Defines.h"
#include "IKeyListener.h"
#include "IMouseListener.h"

class DeviceVK;

class TextureVK;
class SamplerVK;
class CommandBufferVK;
class CameraVK;
class LightVK;
class Mesh;
class GameObjectSimple;
class GameObjectSkyBox;
class GameObject;
class RendererSimple;
class RendererSkyBox;

class VulkanDemo : public Application, public IKeyListener, public IMouseListener
{
public:
	VulkanDemo() {};
	virtual ~VulkanDemo() {};

	virtual void init() override;
	virtual void update(float deltaSeconds) override;
	virtual const std::vector<CommandBufferVK*>& frame() override;
	virtual void shutdown() override;

	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;

	virtual void onMouseButtonPressed(int button) override;
	virtual void onMouseButtonRelease(int button) override;
	virtual void onMouseMove(const glm::vec2& pos, const glm::vec2& offset) override;

private:
	CameraVK* m_Camera;
	LightVK* m_Light;
	
	GameObjectSimple* m_GameObject;
	GameObjectSimple* m_GameObjectGround;
	RendererSimple* m_RendererSimple;

	GameObject* m_GameObjectSkyBox;
	RendererSkyBox* m_RendererSkyBox;

	std::vector<GameObject*> m_SimpleGameObjects;
	std::vector<CommandBufferVK*> m_CommandBuffers;
};
