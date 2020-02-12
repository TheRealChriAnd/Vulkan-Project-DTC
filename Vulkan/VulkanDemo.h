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
class LightVK;

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
	PipelineVK* m_Pipeline;
	DescriptorSetVK* m_DescriptorSet;
	ShaderVK* m_VertexShader;
	ShaderVK* m_FragmentShader;
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferNor;
	StorageBufferVK* m_StorageBufferUV;
	UniformBufferVK* m_UniformBuffer;
	TextureVK* m_Texture;
	SamplerVK* m_Sampler;
	IndexBufferVK* m_IndexBuffer;
	CameraVK* m_Camera;
	LightVK* m_Light;

	std::vector<CommandBufferVK*> m_CommandBuffers;

	float m_Rotation;
};
