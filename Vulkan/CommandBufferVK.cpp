#include "CommandBufferVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "PipelineVK.h"
#include "StorageBufferVK.h"
#include "BufferVK.h"
#include "IndexBufferVK.h"
#include "DescriptorSetVK.h"
#include "CommandPoolVK.h"
#include <iostream>
#include <array>

CommandBufferVK::CommandBufferVK(DeviceVK* device) : CommandBufferVK(device, device->getCommandPool(), 1, true)
{
	
}

CommandBufferVK::CommandBufferVK(DeviceVK* device, CommandPoolVK* commandPool, SwapChainVK* swapChain, bool primary) : CommandBufferVK(device, commandPool, swapChain->getCount(), primary)
{

}

CommandBufferVK::CommandBufferVK(DeviceVK* device, CommandPoolVK* commandPool, int buffers, bool primary)
{
	m_Device = device;
	m_CommandPool = commandPool;
	m_Primary = primary;

	m_CommandBuffers.resize(buffers);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level					= primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandPool			= commandPool->m_CommandPool;
	allocInfo.commandBufferCount	= buffers;

	if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");
}

CommandBufferVK::~CommandBufferVK()
{
	vkFreeCommandBuffers(m_Device->getDevice(), m_CommandPool->m_CommandPool, m_CommandBuffers.size(), m_CommandBuffers.data());
}

void CommandBufferVK::begin(int index, VkCommandBufferUsageFlagBits bufferUsage, RenderPassVK* renderPass) const
{
	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	if (!m_Primary)
	{
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = renderPass->getRenderPass();
		inheritanceInfo.subpass = 0;
		inheritanceInfo.framebuffer = VK_NULL_HANDLE;
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = bufferUsage;
	beginInfo.pInheritanceInfo = m_Primary ? nullptr : &inheritanceInfo;

	if (vkBeginCommandBuffer(m_CommandBuffers[index], &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to begin recording command buffer!");
}

void CommandBufferVK::end(int index) const
{
	if (vkEndCommandBuffer(m_CommandBuffers[index]) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to record command buffer!");
}

void CommandBufferVK::submit() const
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount	= m_CommandBuffers.size();
	submitInfo.pCommandBuffers		= m_CommandBuffers.data();

	vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_Device->getGraphicsQueue());
}

VkCommandBuffer CommandBufferVK::getCommandBuffer(int index) const
{
	return m_CommandBuffers[index];
}

const std::vector<VkCommandBuffer>& CommandBufferVK::getCommandBuffers() const
{
	return m_CommandBuffers;
}

void CommandBufferVK::beginRenderPass(int index, RenderPassVK* renderPass, SwapChainVK* swapChain, const glm::vec4& clearColor, float clearDepth, uint32_t clearStencil) const
{
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	clearValues[1].depthStencil = { clearDepth, clearStencil };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass			= renderPass->getRenderPass();
	renderPassInfo.framebuffer			= swapChain->getFrameBuffers()[index];
	renderPassInfo.renderArea.offset	= { 0, 0 };
	renderPassInfo.renderArea.extent	= swapChain->getExtent();
	renderPassInfo.clearValueCount		= static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues			= clearValues.data();

	vkCmdBeginRenderPass(m_CommandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

void CommandBufferVK::writeSecondaryBuffers(int index, const std::vector<CommandBufferVK*>& buffers)
{
	std::vector<VkCommandBuffer> commandBuffers(buffers.size());
	for (int i = 0; i < buffers.size(); i++)
		commandBuffers[i] = buffers[i]->m_CommandBuffers[index];

	vkCmdExecuteCommands(m_CommandBuffers[index], commandBuffers.size(), commandBuffers.data());
}

void CommandBufferVK::bindPipeline(int index, PipelineVK* pipeline)
{
	vkCmdBindPipeline(m_CommandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
}

void CommandBufferVK::bindVertexBuffers(int index, const std::vector<StorageBufferVK*>& vertexBuffers)
{
	std::vector<VkBuffer> buffers(vertexBuffers.size());
	std::vector<VkDeviceSize> offsets(vertexBuffers.size());

	VkDeviceSize offset = 0;
	for (int i = 0; i < vertexBuffers.size(); i--)
	{
		buffers[i] = vertexBuffers[i]->getVKBuffer();
		offsets[i] = offset;
		offset += vertexBuffers[i]->getSize();
	}

	vkCmdBindVertexBuffers(m_CommandBuffers[index], 0, 1, buffers.data(), offsets.data());
}

void CommandBufferVK::bindIndexBuffer(int index, IndexBufferVK* indexBuffer)
{
	vkCmdBindIndexBuffer(m_CommandBuffers[index], indexBuffer->getVKBuffer(), 0, indexBuffer->getIndexType());
}

void CommandBufferVK::bindDescriptorSets(int index, PipelineVK* pipeline, const std::vector<DescriptorSetVK*>& descriptorSets)
{
	std::vector<VkDescriptorSet> sets(descriptorSets.size());
	for (int i = 0; i < descriptorSets.size(); i++)
		sets[i] = descriptorSets[i]->getDescriptorSet(index);

	vkCmdBindDescriptorSets(m_CommandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getLayout(), 0, sets.size(), sets.data(), 0, nullptr);
}

void CommandBufferVK::drawIndexed(int index, IndexBufferVK* indexBuffer, int instances)
{
	vkCmdDrawIndexed(m_CommandBuffers[index], static_cast<uint32_t>(indexBuffer->getCount()), instances, 0, 0, 0);
}

void CommandBufferVK::draw(int index, int vertices, int instances)
{
	vkCmdDraw(m_CommandBuffers[index], vertices, instances, 0, 0);
}

void CommandBufferVK::endRenderPass(int index)
{
	vkCmdEndRenderPass(m_CommandBuffers[index]);
}
