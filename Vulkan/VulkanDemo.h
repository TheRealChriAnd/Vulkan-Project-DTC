#pragma once
#include "Application.h"
#include "Defines.h"

class DeviceVK;

class TextureVK;
class SamplerVK;
class CommandBufferVK;
class CameraVK;
class Mesh;
class GameObjectSimple;
class RendererSimple;

class VulkanDemo : public Application
{
public:
	VulkanDemo() {};
	virtual ~VulkanDemo() {};

	virtual void init() override;
	virtual void update(float deltaSeconds) override;
	virtual const std::vector<CommandBufferVK*>& frame() override;
	virtual void shutdown() override;

private:
	TextureVK* m_Texture;
	CameraVK* m_Camera;
	SamplerVK* m_Sampler;

	Mesh* m_Mesh;
	GameObjectSimple* m_GameObject;
	RendererSimple* m_RendererSimple;

	std::vector<CommandBufferVK*> m_CommandBuffers;
};
