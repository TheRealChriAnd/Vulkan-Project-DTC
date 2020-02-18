#pragma once
#include <set>
#include <vector>
#include <unordered_map>
#include <typeinfo>

class DeviceVK;
class SwapChainVK;
class RenderPassVK;
class CommandBufferVK;
class GameObject;
class LightVK;
class CameraVK;

class IRenderer
{
public:
	IRenderer(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	virtual ~IRenderer();

	virtual void init() = 0;
	virtual void render(CommandBufferVK* m_CommandBuffer, const std::vector<GameObject*>& gameObjects) = 0;
	virtual void update(float deltaSeconds, CameraVK* camera) = 0;

	void addLight(LightVK* light);
protected:
	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
	RenderPassVK* m_RenderPass;
	std::unordered_map<long long, std::vector<LightVK*>> m_Lights;
};