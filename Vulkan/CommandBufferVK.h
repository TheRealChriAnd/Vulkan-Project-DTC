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
class CommandPoolVK;

class CommandBufferVK
{
public:
	CommandBufferVK(DeviceVK* device);
	CommandBufferVK(DeviceVK* device, CommandPoolVK* commandPool, SwapChainVK* swapChain, bool primary);
	~CommandBufferVK();

	void begin(int index = 0, VkCommandBufferUsageFlagBits bufferUsage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, RenderPassVK* renderPass = nullptr) const;
	void end(int index = 0) const;
	void submit(bool isTransfer) const;

	VkCommandBuffer getCommandBuffer(int index = 0) const;
	const std::vector<VkCommandBuffer>& getCommandBuffers() const;

public:
	void beginRenderPass(int index, RenderPassVK* renderPass, SwapChainVK* swapChain, const glm::vec4& clearColor, float clearDepth, uint32_t clearStencil) const;
	void writeSecondaryBuffers(int index, const std::vector<CommandBufferVK*>& buffers);
	void bindPipeline(int index, PipelineVK* pipeline);
	void bindVertexBuffers(int index, const std::vector<StorageBufferVK*>& vertexBuffers);
	void bindIndexBuffer(int index, IndexBufferVK* indexBuffer);
	void bindDescriptorSets(int index, PipelineVK* pipeline, const std::vector<DescriptorSetVK*>& descriptorSets);
	void drawIndexed(int index, IndexBufferVK* indexBuffer, int instances = 1);
	void draw(int index, int vertices, int instances);
	void endRenderPass(int index);

private:
	CommandBufferVK(DeviceVK* device, CommandPoolVK* commandPool, int buffers, bool primary);

private:
	std::vector<VkCommandBuffer> m_CommandBuffers;
	bool m_Primary;
	DeviceVK* m_Device;
	CommandPoolVK* m_CommandPool;
};
