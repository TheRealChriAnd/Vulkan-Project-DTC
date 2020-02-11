#pragma once
#include "Application.h"
#include "Defines.h"

class DeviceVK;

class DescriptorSetLayoutVK;
class PipelineVK;
class DescriptorSetVK;
class ShaderVK;
class StorageBufferVK;
class UniformBufferVK;
class TextureVK;
class SamplerVK;
class CommandBufferVK;
class IndexBufferVK;
class CameraVK;
class MeshVK;
class GameObject;

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
	DescriptorSetLayoutVK* m_DescriptorSetLayout;
	DescriptorSetLayoutVK* m_DescriptorSetLayout2;
	PipelineVK* m_Pipeline;
	ShaderVK* m_VertexShader;
	ShaderVK* m_FragmentShader;
	UniformBufferVK* m_UniformBuffer;
	TextureVK* m_Texture;
	IndexBufferVK* m_IndexBuffer;
	DescriptorSetVK* m_DescriptorSet2;
	CameraVK* m_Camera;

	MeshVK* m_Mesh;
	GameObject* m_GameObject;

	std::vector<CommandBufferVK*> m_CommandBuffers;
};
