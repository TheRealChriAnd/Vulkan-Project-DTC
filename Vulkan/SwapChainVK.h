#pragma once
#include "Defines.h"
#include <vector>

class RendererVK;
class WindowVK;
class DeviceVK;
class RenderPassVK;

class SwapChainVK
{
public:
	SwapChainVK(WindowVK* window, DeviceVK* device);
	~SwapChainVK();

	VkSwapchainKHR getSwapChain() const;
	VkFormat getImageFormat() const;
	VkFormat getDepthFormat() const;
	VkExtent2D getExtent() const;
	int getCount() const;
	uint32_t getCurrentImageIndex() const;
	const std::vector<VkFramebuffer>& getFrameBuffers() const;

	void createFramebuffers(DeviceVK* device, RenderPassVK* renderPass);

	int acquireNextImage(VkSemaphore semaphore, int currentFrame);

private:
	void createSwapChain(WindowVK* window, DeviceVK* device);
	void createImageViews(DeviceVK* device);
	void createDepthResources(DeviceVK* device);

private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(WindowVK* window, const VkSurfaceCapabilitiesKHR & capabilities);

	VkFormat findDepthFormat(DeviceVK* device);
	VkFormat findSupportedFormat(DeviceVK* device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:
	VkSwapchainKHR m_SwapChain;
	VkFormat m_ImageFormat;
	VkFormat m_DepthFormat;
	VkExtent2D m_Extent;
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;
	std::vector<VkImage> m_Images;
	std::vector<VkImageView> m_ImageViews;
	std::vector<VkFramebuffer> m_Framebuffers;
	uint32_t m_ImageIndex;

	DeviceVK* m_Device;
};