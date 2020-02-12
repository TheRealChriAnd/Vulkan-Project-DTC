#include "SkyBoxVK.h"
#include "TextureVK.h"
#include "DeviceVK.h"
#include "BufferVK.h"
#include "DescriptorSetLayoutVK.h"
#include "DescriptorSetVK.h"
#include "CommandBufferVK.h"
#include "PipelineVK.h"
#include "ShaderVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "MeshVK.h"
#include "SamplerVK.h"
#include "GameObject.h"
#include "stb_image.h"
#include <iostream>
#define IMAGE_COUNT 6


SkyBoxVK::SkyBoxVK(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass) :
	m_Image(nullptr)
{
	m_Device = device;

	compileShader();
	initDescriptorLayout();
	initPipeline(swapChain, renderPass);

	//m_Sampler = new SamplerVK(m_Device);

	m_Mesh = MeshVK::createCube(m_Device);
	m_GameObject = new GameObject(m_DescriptorSetLayout, m_Mesh, this);

	//görs i Gameobject/Mesh
	//m_DescriptorSet = new DescriptorSetVK(m_Device, swapChain, m_DescriptorSetLayout1);
	//m_DescriptorSet->addStorageBuffer(0, m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet->addStorageBuffer(1, m_StorageBufferNor, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet->addStorageBuffer(2, m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	//m_DescriptorSet->addUniformBuffer(3, m_UniformBuffer);
	//m_DescriptorSet->addSkyBoxTexture(5, this, m_SkySampler);
	//m_DescriptorSet->submit();
}

SkyBoxVK::~SkyBoxVK()
{
	VkDevice device = m_Device->getDevice();
	vkDestroyImage(device, m_Image, nullptr);
	vkDestroyImageView(device, m_ImageView, nullptr);
	vkFreeMemory(device, m_ImageMemory, nullptr);
}

int SkyBoxVK::loadFromFile(std::string filename, int index)
{
	createTextureImage(m_Device, filename, IMAGE_COUNT, index);
	
	m_ImageView = m_Device->createImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, IMAGE_COUNT);
	return 0;
}

void SkyBoxVK::bind(unsigned int slot)
{
}

VkImageView SkyBoxVK::getImageView() const
{
	return m_ImageView;
}

GameObject* SkyBoxVK::getGameObject() const
{
	return m_GameObject;
}

MeshVK* SkyBoxVK::getMesh() const
{
	return m_Mesh;
}

DescriptorSetVK* SkyBoxVK::getDescriptorSet() const
{
	return m_DescriptorSet;
}

PipelineVK* SkyBoxVK::getPipeline() const
{
	return m_Pipeline;
}

CommandBufferVK* SkyBoxVK::getCommandBuffer() const
{
	return m_CommandBuffer;
}

void SkyBoxVK::createTextureImage(DeviceVK* device, const std::string& file, uint32_t layers, int index)
{
	int texWidth;
	int texHeight;
	int texChannels;
	stbi_uc* pixels = stbi_load(file.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("Error: Failed to load texture image!");

	BufferVK buffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


	buffer.writeData(pixels, static_cast<size_t>(imageSize));

	stbi_image_free(pixels);

	device->createImage(texWidth, texHeight, layers, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

	transitionImageLayout(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, index);
	copyBufferToImage(device, buffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), index);
	transitionImageLayout(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, index);
}

void SkyBoxVK::transitionImageLayout(DeviceVK* device, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int index)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = index;
	barrier.subresourceRange.layerCount = 6;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer.getCommandBuffer(),
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	commandBuffer.end();
	commandBuffer.submit();
}

void SkyBoxVK::copyBufferToImage(DeviceVK* device, const BufferVK& buffer, uint32_t width, uint32_t height, int index)
{
	CommandBufferVK commandBuffer(device);
	commandBuffer.begin();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = index;
	region.imageSubresource.layerCount = 6;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer.getCommandBuffer(), buffer.getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.end();
	commandBuffer.submit();
}

void SkyBoxVK::compileShader()
{
	m_VertexShader = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_VertexShader->compile();

	m_FragmentShader = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_FragmentShader->compile();
}

void SkyBoxVK::initDescriptorLayout()
{
	m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addStorageBuffer(0, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(2, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addUniformBuffer(3, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addTexture(5, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->submit();
}

void SkyBoxVK::initPipeline(SwapChainVK* swapChain, RenderPassVK* renderPass)
{
	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_DescriptorSetLayout);
	m_Pipeline->addShader(m_VertexShader);
	m_Pipeline->addShader(m_FragmentShader);
	m_Pipeline->submit(m_Device, renderPass, swapChain, VK_POLYGON_MODE_FILL);
}

void SkyBoxVK::recordCommandBuffer(SwapChainVK* swapChain, RenderPassVK* renderPass)
{
	CommandBufferVK* m_CommandBuffer1 = new CommandBufferVK(m_Device, swapChain);
	for (size_t i = 0; i < swapChain->getCount(); i++)
	{
		m_CommandBuffer1->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer1->beginRenderPass(i, renderPass, swapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBuffer1->bindPipeline(i, m_Pipeline);
		m_CommandBuffer1->bindIndexBuffer(i, m_Mesh->m_IndexBuffer);
		m_CommandBuffer1->bindDescriptorSets(i, m_Pipeline, { m_DescriptorSet });
		m_CommandBuffer1->drawIndexed(i, m_Mesh->m_IndexBuffer);
		m_CommandBuffer1->endRenderPass(i);
		m_CommandBuffer1->end(i);
	}
}
