#pragma once
#include "Defines.h"
#include <string>

class DeviceVK;
class BufferVK;
class SwapChainVK;
class RenderPassVK;
class ShaderVK;
class DescriptorSetVK;
class DescriptorSetLayoutVK;
class PipelineVK;
class SamplerVK;
class MeshVK;
class GameObject;
class CommandBufferVK;

class SkyBoxVK
{
public:
	SkyBoxVK(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	~SkyBoxVK();

	int loadFromFile(std::string filename, int index);
	void bind(unsigned int slot);

	VkImageView getImageView() const;
	GameObject* getGameObject() const;
	MeshVK* getMesh() const;
	DescriptorSetVK* getDescriptorSet() const;
	PipelineVK* getPipeline() const;
	CommandBufferVK* getCommandBuffer() const;

private:
	void createTextureImage(DeviceVK* device, const std::string& file, uint32_t layers, int index);
	void transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int index);
	void copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, int index);
	void compileShader();
	void initDescriptorLayout();
	void initPipeline(SwapChainVK* swapChain, RenderPassVK* renderPass);
	void recordCommandBuffer(SwapChainVK* swapChain, RenderPassVK* renderPass);

private:
	DeviceVK* m_Device;

	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_ImageMemory;

	DescriptorSetVK* m_DescriptorSet;
	DescriptorSetLayoutVK* m_DescriptorSetLayout;

	ShaderVK* m_VertexShader;
	ShaderVK* m_FragmentShader;

	PipelineVK* m_Pipeline;
	MeshVK* m_Mesh;
	GameObject* m_GameObject;
	SamplerVK* m_Sampler;
	CommandBufferVK* m_CommandBuffer;

};
