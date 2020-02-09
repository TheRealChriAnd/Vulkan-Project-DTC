#include "RendererVK.h"
#include "DeviceVK.h"
#include "WindowVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "DescriptorSetLayoutVK.h"
#include "PipelineVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "StorageBufferVK.h"
#include "IndexBufferVK.h"
#include "UniformBufferVK.h"
#include "DescriptorSetVK.h"
#include "CommandBufferVK.h"
#include "ShaderVK.h"
#include "MaterialVK.h"
#include "MeshVK.h"
#include <set>
#include <iostream>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

//const int MAX_FRAMES_IN_FLIGHT = 2;
//size_t currentFrame = 0;

RendererVK::RendererVK() :
	m_Window(nullptr),
	m_Device(nullptr),
	m_SwapChain(nullptr),
	m_RenderPass(nullptr)
{
	m_FirstFrame = true;
}

RendererVK::~RendererVK()
{
	
}

MeshVK* RendererVK::makeMesh()
{
	return new MeshVK();
}

StorageBufferVK* RendererVK::makeVertexBuffer(size_t size)
{
	return new StorageBufferVK(m_Device);
}

TextureVK* RendererVK::makeTexture2D()
{
	return new TextureVK(m_Device);
}

SamplerVK* RendererVK::makeSampler2D()
{
	return new SamplerVK(m_Device);
}

std::string RendererVK::getShaderPath()
{
	return std::string("..\\assets\\Vulkan\\");
}

std::string RendererVK::getShaderExtension()
{
	return std::string(".glsl");
}

UniformBufferVK* RendererVK::makeConstantBuffer(std::string NAME, unsigned int location)
{
	UniformBufferVK* buffer = new UniformBufferVK(m_Device, m_SwapChain);
	m_UniformBuffers.push_back(buffer);
	return buffer;
}

int RendererVK::initialize(unsigned int width, unsigned int height)
{
	m_Window = new WindowVK("Vulkan", width, height);
	m_Device = new DeviceVK(m_Window, enableValidationLayers);
	m_SwapChain = new SwapChainVK(m_Window, m_Device);
	m_RenderPass = new RenderPassVK(m_Device, m_SwapChain);

	m_SwapChain->createFramebuffers(m_Device, m_RenderPass);

	/*DescriptorSetLayoutVK* m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addUniformBuffer(TRANSLATION, VK_SHADER_STAGE_VERTEX_BIT);	//5
	m_DescriptorSetLayout->addUniformBuffer(DIFFUSE_TINT, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);	//6
	m_DescriptorSetLayout->addTexture(DIFFUSE_SLOT);		//0
	m_DescriptorSetLayout->submit();
	m_DescriptorSetLayouts.push_back(m_DescriptorSetLayout);

	m_DescriptorSetLayout = new DescriptorSetLayoutVK(m_Device);
	m_DescriptorSetLayout->addStorageBuffer(POSITION, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(NORMAL, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addStorageBuffer(TEXTCOORD, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->submit();
	m_DescriptorSetLayouts.push_back(m_DescriptorSetLayout);*/

	createSyncObjects();

	return 0;
}

void RendererVK::setWinTitle(const char* title)
{
	m_Window->setTitle(title);
}

void RendererVK::present()
{
}

int RendererVK::shutdown()
{
	for (auto& renderState : m_RenderStates)
	{
		delete renderState;
	}

	for (auto& buffer : m_UniformBuffers)
	{
		delete buffer;
	}

	for (auto& descriptorSet : m_DescriptorSets)
	{
		delete descriptorSet;
	}

	delete m_CommandBuffer;

	for (auto& pipeline : m_Pipelines)
	{
		delete pipeline;
	}

	delete m_RenderPass;
	delete m_SwapChain;

	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	//{
	//	vkDestroySemaphore(m_Device->getDevice(), m_RenderFinishedSemaphores[i], nullptr);
	//	vkDestroySemaphore(m_Device->getDevice(), m_ImageAvailableSemaphores[i], nullptr);
	//	vkDestroyFence(m_Device->getDevice(), m_InFlightFences[i], nullptr);
	//}

	for (DescriptorSetLayoutVK* layout : m_DescriptorSetLayouts)
	{
		delete layout;
	}

	delete m_Device;
	delete m_Window;

	delete this;

	return 0;
}

void RendererVK::setClearColor(float r, float g, float b, float a)
{
	m_ColorClear = glm::vec4(r, g, b, a);
}

void RendererVK::clearBuffer(unsigned int)
{
}

void RendererVK::submit(MeshVK* mesh)
{
	m_RenderQueue.push_back(mesh);
}

void RendererVK::frame()
{
	if (m_FirstFrame)
	{
		m_FirstFrame = false;

		m_CommandBuffer = new CommandBufferVK(m_Device, m_SwapChain);

		for (size_t i = 0; i < m_SwapChain->getCount(); i++)
		{
			m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
			m_CommandBuffer->beginRenderPass(i, m_RenderPass, m_SwapChain, m_ColorClear, 1.0F, 0);
		}

		/*for (MeshVK* mesh : m_RenderQueue)
		{
			MeshVK* meshVK = (MeshVK*)mesh;
			m_Pipelines.insert({ mesh->technique, nullptr });

			DescriptorSetVK* m_DescriptorSet0 = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayouts[0]);
			m_DescriptorSet0->addUniformBuffer(TRANSLATION, (UniformBufferVK*)mesh->txBuffer);
			MaterialVK* material = (MaterialVK*)mesh->technique->getMaterial();
			m_DescriptorSet0->addUniformBuffer(DIFFUSE_TINT, material->getUniformBuffer());
			auto textures = mesh->textures;
			if (!textures.empty())
				m_DescriptorSet0->addTexture(DIFFUSE_SLOT, (TextureVK*)textures[0], (SamplerVK*)textures[0]->sampler);


			DescriptorSetVK* m_DescriptorSet1 = new DescriptorSetVK(m_Device, m_SwapChain, m_DescriptorSetLayouts[1]);
			Mesh::VertexBufferBind bind = mesh->geometryBuffers[POSITION];
			m_DescriptorSet1->addStorageBuffer(POSITION, (VertexBufferVK*)bind.buffer, bind.numElements*bind.sizeElement, bind.offset);
			bind = mesh->geometryBuffers[NORMAL];
			m_DescriptorSet1->addStorageBuffer(NORMAL, (VertexBufferVK*)bind.buffer, bind.numElements*bind.sizeElement, bind.offset);
			bind = mesh->geometryBuffers[TEXTCOORD];
			size_t alignment = m_Device->getProperties().limits.minStorageBufferOffsetAlignment;
			m_DescriptorSet1->addStorageBuffer(TEXTCOORD, (VertexBufferVK*)bind.buffer, bind.numElements*bind.sizeElement, bind.offset + (-bind.offset & (alignment - 1)));


			m_DescriptorSet0->submit();
			m_DescriptorSet1->submit();

			meshVK->m_DescriptorSet0 = m_DescriptorSet0;
			meshVK->m_DescriptorSet1 = m_DescriptorSet1;

			m_DescriptorSets.push_back(m_DescriptorSet0);
			m_DescriptorSets.push_back(m_DescriptorSet1);
		}
		
		for (auto& pipeline : m_Pipelines)
		{
			Technique* technique = pipeline.first;
			MaterialVK* material = (MaterialVK*)technique->getMaterial();
			RenderStateVK* renderState = (RenderStateVK*)technique->getRenderState();
			PipelineVK* pipeline = new PipelineVK(m_Device);
			for (ShaderVK* shader : material->getShaders())
				pipeline->addShader(shader);

			pipeline->addDescriptorLayout(m_DescriptorSetLayouts[0]);
			pipeline->addDescriptorLayout(m_DescriptorSetLayouts[1]);

			if (renderState->isWireFrame())
				pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_LINE);
			else
				pipeline->submit(m_Device, m_RenderPass, m_SwapChain, VK_POLYGON_MODE_FILL);

			m_Pipelines[technique] = pipeline;

			createCommandBuffers(technique, pipeline);
		}

		for (size_t i = 0; i < m_SwapChain->getCount(); i++)
		{
			m_CommandBuffer->endRenderPass(i);
			m_CommandBuffer->end(i);
		}*/
	}

	drawFrame();

	m_RenderQueue.clear();
}

void RendererVK::createCommandBuffers(PipelineVK* pipeline)
{
	for (size_t i = 0; i < m_SwapChain->getCount(); i++)
	{
		//m_CommandBuffer->begin(i, (VkCommandBufferUsageFlagBits)0);
		m_CommandBuffer->bindPipeline(i, pipeline);
		////m_CommandBuffer->bindVertexBuffers(i, { m_VertexBuffer });
		for (MeshVK* mesh : m_RenderQueue)
		{
			MeshVK* meshVK = (MeshVK*)mesh;
			/*if (mesh->technique == technique)
			{
				m_CommandBuffer->bindDescriptorSets(i, pipeline, { meshVK->m_DescriptorSet0, meshVK->m_DescriptorSet1});
				m_CommandBuffer->draw(i, 3, 1);
			}*/
		}
		
		/*m_CommandBuffer->endRenderPass(i);
		m_CommandBuffer->end(i);*/
	}
}

void RendererVK::createSyncObjects()
{
	//m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	//m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	//m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	//m_ImagesInFlight.resize(m_SwapChain->getCount(), VK_NULL_HANDLE);

	//VkSemaphoreCreateInfo semaphoreInfo = {};
	//semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//VkFenceCreateInfo fenceInfo = {};
	//fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	//{
	//	if (vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
	//		vkCreateSemaphore(m_Device->getDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
	//		vkCreateFence(m_Device->getDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("Error: Failed to create synchronization objects for a frame!");
	//	}
	//}
}

void RendererVK::drawFrame()
{
	//vkWaitForFences(m_Device->getDevice(), 1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//VkResult result = m_SwapChain->acquireNextImage(m_ImageAvailableSemaphores[currentFrame], currentFrame);
	//uint32_t imageIndex = m_SwapChain->getCurrentImageIndex();
	///*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	//	recreateSwapChain();
	//	return;
	//}
	//else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	//	throw std::runtime_error("failed to acquire swap chain image!");
	//}*/

	//if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
	//{
	//	vkWaitForFences(m_Device->getDevice(), 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	//}
	//m_ImagesInFlight[imageIndex] = m_InFlightFences[currentFrame];

	//VkSubmitInfo submitInfo = {};
	//submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
	//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	//submitInfo.waitSemaphoreCount = 1;
	//submitInfo.pWaitSemaphores = waitSemaphores;
	//submitInfo.pWaitDstStageMask = waitStages;

	//submitInfo.commandBufferCount = 1;
	//submitInfo.pCommandBuffers = &m_CommandBuffer->getCommandBuffers().data()[imageIndex];

	//VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[currentFrame] };
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = signalSemaphores;

	//vkResetFences(m_Device->getDevice(), 1, &m_InFlightFences[currentFrame]);

	//if (vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS)
	//	throw std::runtime_error("Error: Failed to submit draw command buffer!");

	//VkPresentInfoKHR presentInfo = {};
	//presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	//presentInfo.waitSemaphoreCount = 1;
	//presentInfo.pWaitSemaphores = signalSemaphores;

	//VkSwapchainKHR swapChains[] = { m_SwapChain->getSwapChain() };
	//presentInfo.swapchainCount = 1;
	//presentInfo.pSwapchains = swapChains;

	//presentInfo.pImageIndices = &imageIndex;

	//result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

	///*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	//	framebufferResized = false;
	//	recreateSwapChain();
	//}
	//else if (result != VK_SUCCESS) {
	//	throw std::runtime_error("failed to present swap chain image!");
	//}*/

	//currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}