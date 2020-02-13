#pragma once
#include "Application.h"
#include "Defines.h"

class DeviceVK;

class TextureVK;
class SamplerVK;
class CommandBufferVK;
class CameraVK;
class LightVK;
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
	CameraVK* m_Camera;
	LightVK* m_Light;
	SamplerVK* m_Sampler;

	TextureVK* m_Texture;
	TextureVK* m_TextureGround;
	Mesh* m_Mesh;
	Mesh* m_MeshGround;
	GameObjectSimple* m_GameObject;
	GameObjectSimple* m_GameObjectGround;
	RendererSimple* m_RendererSimple;

	std::vector<CommandBufferVK*> m_CommandBuffers;
};
