#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "WindowVK.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "RenderPassVK.h"
#include "CommandBufferVK.h"
#include "InputVK.h"
#include "UniformBufferVK.h"
#include "RES.h"
#include "ThreadManager.h"


#include <chrono>
#include <iostream>

#include "Profiler.h"

Application::Application()
{
	Profiler::begin("INIT");
	ThreadManager::init();
}

void Application::run()
{
	m_Window = new WindowVK("Vulkan Project", 1080, 720);
	m_Device = new DeviceVK(m_Window, enableValidationLayers);

	Profiler::begin("RESOURCES");
	RES::init(m_Device);
	Profiler::end();

	Profiler::begin("PRE_INIT");
	this->preInit();
	Profiler::end();

	createSwapChainInternal();

	Profiler::begin("SYNC_OBJECTS");
	createSyncObjects();
	Profiler::end();

	InputVK::init(m_Window);
	this->init();
	Profiler::end();

	Profiler::printResults();

	float debugTimer = 0;
	int frames = 0;
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!m_Window->shouldClose())
	{
		Profiler::begin("EVENTS");
		m_Window->pollEvents();
		Profiler::end();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;
		debugTimer += delta;

		m_Device->waitForFence(m_InFlightFences[currentFrame]);
		uint32_t imageIndex = m_SwapChain->acquireNextImage(m_ImageAvailableSemaphores[currentFrame], currentFrame);

		if (imageIndex == -1)
		{
			recreateSwapChain();
			continue;
		}

		Profiler::begin("UPDATE");
		this->update(delta);
		Profiler::end();

		Profiler::begin("UNIFORM_BUFFER");
		UniformBufferVK::transfer();
		Profiler::end();

		Profiler::begin("FRAME");
		drawFrame(imageIndex);
		Profiler::end();
		Profiler::count("FRAMES");

		frames++;
		if (debugTimer >= 1.0F)
		{
			debugTimer -= 1.0F;
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
	}
	Profiler::printResults();

	Profiler::begin("SHUTDOWN");
	shutdownInternal();
	Profiler::end();

	Profiler::printResults();
}

void Application::shutdownInternal()
{
	ThreadManager::shutdown();

	m_Device->waitForIdle();

	releaseSwapChainInternal();
	this->shutdown();
	RES::shutdown();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_Device->getDevice(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device->getDevice(), m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_Device->getDevice(), m_InFlightFences[i], nullptr);
	}

	delete m_Device;
	delete m_Window;
}

void Application::createSwapChainInternal()
{
	Profiler::begin("CREATE");
	m_SwapChain = new SwapChainVK(m_Window, m_Device);
	m_RenderPass = new RenderPassVK(m_Device, m_SwapChain);
	m_SwapChain->createFramebuffers(m_Device, m_RenderPass);
	this->onSwapChainCreated();
	Profiler::end();
}

void Application::releaseSwapChainInternal()
{
	this->onSwapChainReleased();
	delete m_RenderPass;
	delete m_SwapChain;
}

void Application::recreateSwapChain()
{
	m_Device->waitForIdle();
	releaseSwapChainInternal();
	createSwapChainInternal();
}

void Application::createSyncObjects()
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

void Application::drawFrame(uint32_t imageIndex)
{
	VkFence lastFence = m_ImagesInFlight[imageIndex];
	if (lastFence != VK_NULL_HANDLE)
	{
		m_Device->waitForFence(lastFence);
	}

	VkFence newFence = m_InFlightFences[currentFrame];

	m_ImagesInFlight[imageIndex] = newFence;

	VkCommandBuffer commandBuffer = this->frame()->getCommandBuffer(imageIndex);
	VkSemaphore waitSemaphore = m_ImageAvailableSemaphores[currentFrame];
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSemaphore signalSemaphore = m_RenderFinishedSemaphores[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore;
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore;

	m_Device->resetFence(newFence);

	m_Device->submitToGraphicsQueue(submitInfo, newFence);

	m_SwapChain->present(signalSemaphore);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}