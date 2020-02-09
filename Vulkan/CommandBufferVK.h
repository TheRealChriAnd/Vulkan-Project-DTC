#pragma once
#include "Defines.h"
#include <vector>
#include "glm/glm.hpp"

class DeviceVK;
class SwapChainVK;
class RenderPassVK;
class PipelineVK;
class StorageBufferVK;
class IndexBufferVK;
class DescriptorSetVK;

class CommandBufferVK
{
public:
	CommandBufferVK(DeviceVK* device, SwapChainVK* swapChain);
	CommandBufferVK(DeviceVK* device);
	~CommandBufferVK();

	void begin(int index = 0, VkCommandBufferUsageFlagBits bufferUsage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) const;
	void end(int index = 0) const;
	void submit() const;

	VkCommandBuffer getCommandBuffer(int index = 0) const;
	const std::vector<VkCommandBuffer>& getCommandBuffers() const;

public:
	void beginRenderPass(int index, RenderPassVK* renderPass, SwapChainVK* swapChain, const glm::vec4& clearColor, float clearDepth, uint32_t clearStencil) const;
	void bindPipeline(int index, PipelineVK* pipeline);
	void bindVertexBuffers(int index, const std::vector<StorageBufferVK*>& vertexBuffers);
	void bindIndexBuffer(int index, IndexBufferVK* indexBuffer);
	void bindDescriptorSets(int index, PipelineVK* pipeline, const std::vector<DescriptorSetVK*>& descriptorSets);
	void drawIndexed(int index, IndexBufferVK* indexBuffer, int instances = 1);
	void draw(int index, int vertices, int instances);
	void endRenderPass(int index);

private:
	CommandBufferVK(DeviceVK* device, int buffers);

private:
	std::vector<VkCommandBuffer> m_CommandBuffers;

	DeviceVK* m_Device;
};
