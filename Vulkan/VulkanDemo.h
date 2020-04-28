#pragma once
#include <atomic>
#include "Application.h"
#include "Defines.h"
#include "IKeyListener.h"
#include "IMouseListener.h"
#include "ICameraListener.h"

class DeviceVK;

class TextureVK;
class TextureAnimated;
class SamplerVK;
class CommandBufferVK;
class CameraVK;
class LightVK;
class LightPoint;
class Mesh;
class GameObjectSimple;
class GameObjectSkyBox;
class GameObject;
class RendererSimple;
class RendererSkyBox;
class CommandPoolVK;

class VulkanDemo : public Application, public IKeyListener, public IMouseListener, public ICameraListener
{
public:
	VulkanDemo(int tvs) : Application(tvs) {};
	virtual ~VulkanDemo() {};

	virtual void preInit() override;
	virtual void init() override;
	void createCommandBuffers(size_t index);
	virtual void update(float deltaSeconds) override;
	virtual CommandBufferVK* frame() override;
	virtual void shutdown() override;
	
	virtual void onSwapChainReleased() override;
	virtual void onSwapChainCreated() override;

	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;

	virtual void onMouseButtonPressed(int button) override;
	virtual void onMouseButtonRelease(int button) override;
	virtual void onMouseMove(const glm::vec2& pos, const glm::vec2& offset) override;

	virtual void onCameraPathEnded(CameraVK* camera) override;

	void onTVFrameReady(TextureAnimated* texture);
	void drawSimpleGameObjects(size_t index, std::atomic_bool& done);

private:
	CameraVK* m_Camera;
	std::vector<LightPoint*> m_PointLight;

	GameObjectSimple* m_GameObjectSofa;
	GameObjectSimple* m_GameObjectRightWall;
	GameObjectSimple* m_GameObjectFrontWall;
	GameObjectSimple* m_GameObjectWindow;
	GameObjectSimple* m_GameObjectGround;
	GameObjectSimple* m_GameObjectFloor;
	GameObjectSimple* m_GameObjectFloor2;
	GameObjectSimple* m_GameObjectWallRoom2Front;
	GameObjectSimple* m_GameObjectWallRoom2Left;



	GameObjectSimple* m_GameObjectTv;
	GameObjectSimple* m_GameObjectTable;
	RendererSimple* m_RendererSimple;

	GameObject* m_GameObjectSkyBox;
	RendererSkyBox* m_RendererSkyBox;

	std::vector<GameObject*> m_SimpleGameObjects;

	CommandBufferVK* m_CommandBufferPrimary;
	CommandBufferVK* m_CommandBufferSkyBox;
	CommandBufferVK* m_CommandBufferSimple;

	CommandPoolVK* m_GraphicsCommandPool2;

	bool m_BenchmarkRunning;
};
