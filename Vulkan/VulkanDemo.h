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
class SkyBoxVK;

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
	DescriptorSetLayoutVK* m_DescriptorSetLayout1;
	PipelineVK* m_Pipeline;
	PipelineVK* m_Pipeline1;
	DescriptorSetVK* m_DescriptorSet;
	DescriptorSetVK* m_DescriptorSet1;
	ShaderVK* m_VertexShader;
	ShaderVK* m_FragmentShader;
	ShaderVK* m_VertexShader1;
	ShaderVK* m_FragmentShader1;
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferNor;
	StorageBufferVK* m_StorageBufferUV;
	UniformBufferVK* m_UniformBuffer;
	StorageBufferVK* m_StorageBufferPos1;
	StorageBufferVK* m_StorageBufferNor1;
	StorageBufferVK* m_StorageBufferUV1;
	UniformBufferVK* m_UniformBuffer1;
	TextureVK* m_Texture;
	SamplerVK* m_Sampler;
	IndexBufferVK* m_IndexBuffer;
	IndexBufferVK* m_IndexBuffer1;
	CameraVK* m_Camera;
	SkyBoxVK* m_Skybox;
	SamplerVK* m_SkySampler;

	std::vector<CommandBufferVK*> m_CommandBuffers;

	float m_Rotation;
};
