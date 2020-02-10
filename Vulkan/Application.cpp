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

#include <chrono>

Application::Application()
{
	
}

void Application::run()
{
	m_Window = new WindowVK("Vulkan Project", 800, 600);
	m_Device = new DeviceVK(m_Window, enableValidationLayers);
	m_SwapChain = new SwapChainVK(m_Window, m_Device);
	m_RenderPass = new RenderPassVK(m_Device, m_SwapChain);

	m_SwapChain->createFramebuffers(m_Device, m_RenderPass);

	createSyncObjects();

	InputVK::init(m_Window);
	this->init();

	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!m_Window->shouldClose())
	{
		m_Window->pollEvents();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		m_Device->waitForFence(&m_InFlightFences[currentFrame]);
		uint32_t imageIndex = m_SwapChain->acquireNextImage(m_ImageAvailableSemaphores[currentFrame], currentFrame);

		this->update(delta);

		drawFrame(imageIndex);
	}

	shutdownInternal();
}

void Application::shutdownInternal()
{
	vkDeviceWaitIdle(m_Device->getDevice());

	this->shutdown();

	delete m_RenderPass;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_Device->getDevice(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device->getDevice(), m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_Device->getDevice(), m_InFlightFences[i], nullptr);
	}

	delete m_SwapChain;
	delete m_Device;
	delete m_Window;
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
	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		m_Device->waitForFence(&m_ImagesInFlight[imageIndex]);
	}

	m_ImagesInFlight[imageIndex] = m_InFlightFences[currentFrame];

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	const std::vector<CommandBufferVK*>& commandBuffers = this->frame();
	std::vector<VkCommandBuffer> buffers(commandBuffers.size());
	for (int i = 0; i < commandBuffers.size(); i++)
		buffers[i] = commandBuffers[i]->getCommandBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = buffers.size();
	submitInfo.pCommandBuffers = buffers.data();

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

	VkResult result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

	/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}*/

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
