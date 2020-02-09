#pragma once
#include "Defines.h"
#include <vector>
#include <optional>
#include <set>

class WindowVK;

struct QueueFamilyIndices
{
	std::optional<uint32_t> m_GraphicsFamily;
	std::optional<uint32_t> m_PresentFamily;

	bool isComplete()
	{
		return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
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
	VkCommandPool getCommandPool() const;
	VkDescriptorPool getDescriptorPool() const;
	VkQueue getGraphicsQueue() const;
	VkQueue getPresentQueue() const;

	VkPhysicalDeviceProperties getProperties() const;

	SwapChainSupportDetails querySwapChainSupport();
	QueueFamilyIndices findQueueFamilies();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkInstance getInstance() const;

private:
	void createInstance(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extentions);
	void setupValidationLayer();
	void pickPhysicalDevice(const std::vector<const char*>& extentions);
	void createLogicalDevice(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extentions);
	void createCommandPool();
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
	VkCommandPool m_CommandPool;
	VkDescriptorPool m_DescriptorPool;

	VkPhysicalDeviceProperties m_Properties;

	VkDebugUtilsMessengerEXT m_DebugMessenger;
	bool m_UseValidationLayers;
};