#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <chrono>

#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "DescriptorSetLayoutVK.h"
#include "PipelineVK.h"
#include "DescriptorSetVK.h"
#include "ShaderVK.h"
#include "VertexBufferVK.h"
#include "UniformBufferVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "CommandBufferVK.h"
#include "IndexBufferVK.h"

const int MAX_FRAMES_IN_FLIGHT = 2;
size_t currentFrame = 0;

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4

std::vector<glm::vec3> pos = {
	{-0.5f, -0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f},
	{0.5f, 0.5f, 0.0f},
	{-0.5f, 0.5f, 0.0f},

	{-0.5f, -0.5f, -0.5f},
	{0.5f, -0.5f, -0.5f},
	{0.5f, 0.5f, -0.5f},
	{-0.5f, 0.5f, -0.5f}
};

std::vector<glm::vec3> nor = {
	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},

	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f}
};

std::vector<glm::vec2> uv = {
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f},

	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

WindowVK* m_Window;
DeviceVK* m_Device;
SwapChainVK* m_SwapChain;
RenderPassVK* m_RenderPass;
DescriptorSetLayoutVK* m_DescriptorSetLayout;
PipelineVK* m_Pipeline;
DescriptorSetVK* m_DescriptorSet;
ShaderVK* m_VertexShader;
ShaderVK* m_FragmentShader;
VertexBufferVK* m_StorageBufferPos;
VertexBufferVK* m_StorageBufferNor;
VertexBufferVK* m_StorageBufferUV;
UniformBufferVK* m_UniformBuffer;
TextureVK* m_Texture;
SamplerVK* m_Sampler;
CommandBufferVK* m_CommandBuffer;
IndexBufferVK* m_IndexBuffer;
std::vector<VkSemaphore> m_ImageAvailableSemaphores;
std::vector<VkSemaphore> m_RenderFinishedSemaphores;
std::vector<VkFence> m_InFlightFences;
std::vector<VkFence> m_ImagesInFlight;

void drawFrame();
void createCommandBuffers();
void createSyncObjects();
void updateUniformBuffer(uint32_t currentImage);

int main()
{
	m_Window = new WindowVK("Vulkan Project", 800, 600);
	m_Device = new DeviceVK(m_Window, enableValidationLayers);
	m_SwapChain = new SwapChainVK(m_Window, m_Device);
	m_RenderPass = new RenderPassVK(m_Device, m_SwapChain);

	m_SwapChain->createFramebuffers(m_Device, m_RenderPass);

	m_VertexShader = new ShaderVK(m_Device, "shaders/VertexShader.glsl", VK_SHADER_STAGE_VERTEX_BIT);
	m_FragmentShader = new ShaderVK(m_Device, "shaders/FragmentShader.glsl", VK_SHADER_STAGE_FRAGMENT_BIT);
	m_VertexShader->compile();
	m_FragmentShader->compile();


	m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_POS, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_COLOR, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(BINDING_UV, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addUniformBuffer(BINDING_UNI, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addTexture(BINDING_TEXTURE, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->submit();

	m_Pipeline = new PipelineVK(m_Device);
	m_Pipeline->addDescriptorLayout(m_DescriptorSetLayout);
	m_Pipeline->addShader(m_VertexShader);
	m_Pipeline->addShader(m_FragmentShader);
	m_Pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

	m_Texture = new TextureVK(m_Device);
	m_Texture->loadFromFile("textures/fatboy.png");
	m_Sampler = new SamplerVK(m_Device);
	m_StorageBufferPos = new VertexBufferVK(m_Device);
	m_StorageBufferNor = new VertexBufferVK(m_Device);
	m_StorageBufferUV = new VertexBufferVK(m_Device);
	m_StorageBufferPos->setData(pos.data(), sizeof(glm::vec3) * pos.size(), 0);
	m_StorageBufferNor->setData(nor.data(), sizeof(glm::vec3) * nor.size(), 0);
	m_StorageBufferUV->setData(uv.data(), sizeof(glm::vec2) * uv.size(), 0);

	m_IndexBuffer = new IndexBufferVK(m_Device, indices);
	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain);
	m_UniformBuffer->setBufferSize(sizeof(UniformBufferObject));

	m_DescriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayout);
	m_DescriptorSet->addStorageBuffer(BINDING_POS, m_StorageBufferPos, m_StorageBufferPos->getSize() / 2, 48);
	m_DescriptorSet->addStorageBuffer(BINDING_COLOR, m_StorageBufferNor, m_StorageBufferNor->getSize() / 2, 48);
	m_DescriptorSet->addStorageBuffer(BINDING_UV, m_StorageBufferUV, m_StorageBufferUV->getSize() / 2, 32);
	m_DescriptorSet->addUniformBuffer(BINDING_UNI, m_UniformBuffer);
	m_DescriptorSet->addTexture(BINDING_TEXTURE, m_Texture, m_Sampler);
	m_DescriptorSet->submit();

	createCommandBuffers();
	createSyncObjects();

	while (!m_Window->shouldClose())
	{
		m_Window->pollEvents();
		drawFrame();
	}

	return 0;
}

void createCommandBuffers()
{
	m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);

	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0F, 0);
		m_CommandBuffer->bindPipeline(i, m_Pipeline);
		m_CommandBuffer->bindIndexBuffer(i, m_IndexBuffer);
		m_CommandBuffer->bindDescriptorSets(i, m_Pipeline, { m_DescriptorSet });
		m_CommandBuffer->drawIndexed(i, m_IndexBuffer);
		m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);
	}
}

void createSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_ImagesInFlight.resize(m_SwapChain->getCount(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_Device->getDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error: Failed to create synchronization objects for a frame!");
		}
	}
}

void drawFrame()
{
	vkWaitForFences(m_Device->getDevice(), 1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_Device->getDevice(), m_SwapChain->getSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	/*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}*/

	updateUniformBuffer(imageIndex);

	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_Device->getDevice(), 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	m_ImagesInFlight[imageIndex] = m_InFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer->getCommandBuffers().data()[imageIndex];

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_Device->getDevice(), 1, &m_InFlightFences[currentFrame]);

	if (vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

	/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}*/

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->getExtent().width / (float)m_SwapChain->getExtent().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(m_Device->getDevice(), m_UniformBuffer->getBufferMemory(currentImage), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_Device->getDevice(), m_UniformBuffer->getBufferMemory(currentImage));
}