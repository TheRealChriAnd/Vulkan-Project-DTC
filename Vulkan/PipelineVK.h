#pragma once
#include "Defines.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

class DeviceVK;
class RenderPassVK;
class DescriptorSetLayoutVK;
class SwapChainVK;
class ShaderVK;

class PipelineVK
{
public:
	PipelineVK(DeviceVK* device);
	~PipelineVK();

	VkPipeline getPipeline() const;
	VkPipelineLayout getLayout() const;

	void addShader(ShaderVK* shader);
	void addDescriptorLayout(DescriptorSetLayoutVK* descriptorSetLayout);

	void submit(DeviceVK* device, RenderPassVK* renderPass, SwapChainVK* swapChain, VkPolygonMode polygonMode);

private:
	void createGraphicsPipeline(DeviceVK* device, RenderPassVK* renderPass, SwapChainVK* swapChain, VkPolygonMode polygonMode);

private:
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_Pipeline;
	std::vector<VkPipelineShaderStageCreateInfo> m_Shaders;
	std::vector<VkDescriptorSetLayout> m_Layouts;

	DeviceVK* m_Device;
};