#pragma once
#include "Defines.h"
#include <vector>
#include <optional>
#include <set>

class WindowVK;
class CommandPoolVK;

struct QueueFamilyIndices
{
	std::optional<uint32_t> m_GraphicsFamily;
	std::optional<uint32_t> m_PresentFamily;
	std::optional<uint32_t> m_TransferFamily;

	bool isComplete()
	{
		return m_GraphicsFamily.has_value() && m_PresentFamily.has_value() && m_TransferFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR m_Capabilities;
	std::vector<VkSurfaceFormatKHR> m_Formats;
	std::vector<VkPresentModeKHR> m_PresentModes;

	bool isComplete()
	{
		return !m_Formats.empty() && !m_PresentModes.empty();
	}
};

class DeviceVK
{
	friend class WindowVK;
public:
	DeviceVK(WindowVK* window, bool enableValidationLayers);
	~DeviceVK();

	VkDevice getDevice() const;
	VkPhysicalDevice getPhysicalDevice() const;
	VkSurfaceKHR getSurface() const;
	CommandPoolVK* getGraphicsCommandPool() const;
	CommandPoolVK* getTransferCommandPool() const;
	VkDescriptorPool getDescriptorPool() const;
	VkQueue getGraphicsQueue() const;
	VkQueue getTransferQueue() const;
	VkQueue getPresentQueue() const;

	VkPhysicalDeviceProperties getProperties() const;

	void waitForIdle() const;
	void waitForFence(const VkFence fence) const;
	void resetFence(const VkFence fence) const;
	void submitToGraphicsQueue(const VkSubmitInfo& submitInfo, const VkFence fence) const;
	void submitToTransferQueue(const VkSubmitInfo& submitInfo, const VkFence fence) const;

	SwapChainSupportDetails querySwapChainSupport();
	QueueFamilyIndices findQueueFamilies();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32_t layers);
	void createImage(uint32_t width, uint32_t height, uint32_t layers, VkImageCreateFlags flags, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkInstance getInstance() const;

private:
	void createInstance(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extentions);
	void setupValidationLayer();
	void pickPhysicalDevice(const std::vector<const char*>& extentions);
	void createLogicalDevice(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extentions);
	void createDescriptorPool();


private:
	bool isDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& extentions);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& extentions);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
	void destroyDebugUtilsMessengerEXT();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

private:
	VkInstance m_Instance;
	VkSurfaceKHR m_Surface;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;
	VkQueue m_TransferQueue;
	VkDescriptorPool m_DescriptorPool;
	CommandPoolVK* m_GraphicsCommandPool;
	CommandPoolVK* m_TransferCommandPool;

	VkPhysicalDeviceProperties m_Properties;

	VkDebugUtilsMessengerEXT m_DebugMessenger;
	bool m_UseValidationLayers;
};