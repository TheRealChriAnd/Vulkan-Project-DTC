#include "SwapChainVK.h"
#include "RendererVK.h"
#include "WindowVK.h"
#include "DeviceVK.h"
#include "RenderPassVK.h"
#include <algorithm>
#include <array>
#include <iostream>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

SwapChainVK::SwapChainVK(WindowVK* window, DeviceVK* device) :
	m_SwapChain(nullptr),
	m_DepthImageView(nullptr),
	m_ImageIndex(0)
{
	m_Device = device;

	createSwapChain(window, device);
	createImageViews(device);
	createDepthResources(device);
}

SwapChainVK::~SwapChainVK()
{
	VkDevice device = m_Device->getDevice();

	for (int i = 0; i < m_Images.size(); i++)
	{
		vkDestroyImageView(device, m_ImageViews[i], nullptr);
		vkDestroyFramebuffer(device, m_Framebuffers[i], nullptr);
	}

	vkDestroyImage(device, m_DepthImage, nullptr);
	vkFreeMemory(device, m_DepthImageMemory, nullptr);
	vkDestroyImageView(device, m_DepthImageView, nullptr);
	vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
}

VkSwapchainKHR SwapChainVK::getSwapChain() const
{
	return m_SwapChain;
}

VkFormat SwapChainVK::getImageFormat() const
{
	return m_ImageFormat;
}

VkFormat SwapChainVK::getDepthFormat() const
{
	return m_DepthFormat;
}

VkExtent2D SwapChainVK::getExtent() const
{
	return m_Extent;
}

int SwapChainVK::getCount() const
{
	return m_Images.size();
}

uint32_t SwapChainVK::getCurrentImageIndex() const
{
	return m_ImageIndex;
}

const std::vector<VkFramebuffer>& SwapChainVK::getFrameBuffers() const
{
	return m_Framebuffers;
}

void SwapChainVK::createSwapChain(WindowVK* window, DeviceVK* device)
{
	VkDevice vkDevice = device->getDevice();
	SwapChainSupportDetails swapChainSupport = device->querySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.m_Formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.m_PresentModes);
	VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.m_Capabilities);

	uint32_t imageCount = swapChainSupport.m_Capabilities.minImageCount + 1;
	if (swapChainSupport.m_Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_Capabilities.maxImageCount)
		imageCount = swapChainSupport.m_Capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device->getSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = device->findQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value() };

	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (indices.m_GraphicsFamily != indices.m_PresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	createInfo.preTransform = swapChainSupport.m_Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create swap chain!");

	vkGetSwapchainImagesKHR(vkDevice, m_SwapChain, &imageCount, nullptr);
	m_Images.resize(imageCount);
	vkGetSwapchainImagesKHR(vkDevice, m_SwapChain, &imageCount, m_Images.data());

	m_ImageFormat = surfaceFormat.format;
	m_Extent = extent;
}

void SwapChainVK::createImageViews(DeviceVK* device)
{
	m_ImageViews.resize(m_Images.size());

	for (size_t i = 0; i < m_Images.size(); i++)
		m_ImageViews[i] = device->createImageView(m_Images[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
}

void SwapChainVK::createDepthResources(DeviceVK* device)
{
	m_DepthFormat = findDepthFormat(device);
	device->createImage(m_Extent.width, m_Extent.height, 1, m_DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
	m_DepthImageView = device->createImageView(m_DepthImage, m_DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);
}

void SwapChainVK::createFramebuffers(DeviceVK* device, RenderPassVK* renderPass)
{
	m_Framebuffers.resize(m_ImageViews.size());

	for (size_t i = 0; i < m_ImageViews.size(); i++)
	{
		std::array<VkImageView, 2> attachments =
		{
			m_ImageViews[i],
			m_DepthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= renderPass->getRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments	= attachments.data();
		framebufferInfo.width			= m_Extent.width;
		framebufferInfo.height			= m_Extent.height;
		framebufferInfo.layers			= 1;

		if (vkCreateFramebuffer(device->getDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}

int SwapChainVK::acquireNextImage(VkSemaphore semaphore, int currentFrame)
{
	VkResult result = vkAcquireNextImageKHR(m_Device->getDevice(), m_SwapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &m_ImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//recreateSwapChain();
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	return m_ImageIndex;
}

VkFormat SwapChainVK::findDepthFormat(DeviceVK* device)
{
	return findSupportedFormat(
		device,
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat SwapChainVK::findSupportedFormat(DeviceVK* device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	throw std::runtime_error("Error: Failed to find supported format!");
}

VkSurfaceFormatKHR SwapChainVK::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return availableFormats[0];
}

VkPresentModeKHR SwapChainVK::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChainVK::chooseSwapExtent(WindowVK* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(window->getFramebufferWidth()),
			static_cast<uint32_t>(window->getFramebufferHeight())
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}