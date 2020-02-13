#pragma once
#include "Defines.h"
#include "stb_image.h"
#include <string>
#include <vector>
class DeviceVK;
class BufferVK;
class SwapChainVK;
class RenderPassVK;
class ShaderVK;
class DescriptorSetVK;
class DescriptorSetLayoutVK;
class PipelineVK;
class SamplerVK;
class Mesh;
class GameObject;
class CommandBufferVK;

class SkyBoxVK
{
public:
	SkyBoxVK(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass);
	~SkyBoxVK();

	int loadFromFile(std::string filename, int index);
	void bind(unsigned int slot);

	GameObject* getGameObject() const;
	Mesh* getMesh() const;
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
	void copyPixelData(stbi_uc* data, uint64_t offset, uint64_t size);
	void createRegion(int index, VkOffset3D offset);
	void initImage();

private:
	DeviceVK* m_Device;

	//VkImage m_Image;
	//VkImageView m_ImageView;
	//VkDeviceMemory m_ImageMemory;

	DescriptorSetVK* m_DescriptorSet;
	DescriptorSetLayoutVK* m_DescriptorSetLayout;

	ShaderVK* m_VertexShader;
	ShaderVK* m_FragmentShader;

	PipelineVK* m_Pipeline;
	Mesh* m_Mesh;
	GameObject* m_GameObject;
	SamplerVK* m_Sampler;
	CommandBufferVK* m_CommandBuffer;

	VkDeviceSize m_Size;
	int m_TexWidth;
	int m_TexHeight;

	bool m_FirstImage;
	std::vector<VkBufferImageCopy> bufferCopyRegions;
	stbi_uc* m_PixelData;
};
