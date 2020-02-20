#include "DeviceVK.h"
#include "WindowVK.h"
#include "CommandPoolVK.h"
#include <iostream>
#include <array>

DeviceVK::DeviceVK(WindowVK* window, bool enableValidationLayers) :
	m_UseValidationLayers(enableValidationLayers),
	m_Instance(nullptr),
	m_PhysicalDevice(nullptr),
	m_Device(nullptr),
	m_Surface(nullptr),
	m_GraphicsQueue(nullptr),
	m_PresentQueue(nullptr),
	m_DebugMessenger(nullptr)
{
	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_RENDERDOC_Capture" 
	};

	const std::vector<const char*> instanceExtentions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	const std::vector<const char*> deviceExtentions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	createInstance(validationLayers, instanceExtentions);
	setupValidationLayer();
	window->createSurface(this);
	pickPhysicalDevice(deviceExtentions);
	createLogicalDevice(validationLayers, deviceExtentions);
	m_GraphicsCommandPool = new CommandPoolVK(this, false);
	m_TransferCommandPool = new CommandPoolVK(this, true);
	createDescriptorPool();
}

DeviceVK::~DeviceVK()
{
	if (m_UseValidationLayers)
		destroyDebugUtilsMessengerEXT();

	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
	delete m_GraphicsCommandPool;
	delete m_TransferCommandPool;
	vkDestroyDevice(m_Device, nullptr);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
}

VkDevice DeviceVK::getDevice() const
{
	return m_Device;
}

VkPhysicalDevice DeviceVK::getPhysicalDevice() const
{
	return m_PhysicalDevice;
}

VkSurfaceKHR DeviceVK::getSurface() const
{
	return m_Surface;
}

CommandPoolVK* DeviceVK::getGraphicsCommandPool() const
{
	return m_GraphicsCommandPool;
}

CommandPoolVK* DeviceVK::getTransferCommandPool() const
{
	return m_TransferCommandPool;
}

VkDescriptorPool DeviceVK::getDescriptorPool() const
{
	return m_DescriptorPool;
}

VkQueue DeviceVK::getGraphicsQueue() const
{
	return m_GraphicsQueue;
}

VkQueue DeviceVK::getTransferQueue() const
{
	return m_TransferQueue;
}

VkQueue DeviceVK::getPresentQueue() const
{
	return m_PresentQueue;
}

VkPhysicalDeviceProperties DeviceVK::getProperties() const
{
	return m_Properties;
}

void DeviceVK::waitForIdle() const
{
	vkDeviceWaitIdle(m_Device);
}

void DeviceVK::waitForFence(const VkFence fence) const
{
	vkWaitForFences(m_Device, 1, &fence, VK_TRUE, UINT64_MAX);
}

void DeviceVK::resetFence(const VkFence fence) const
{
	vkResetFences(m_Device, 1, &fence);
}

void DeviceVK::submitToGraphicsQueue(const VkSubmitInfo& submitInfo, const VkFence fence) const
{
	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to submit draw command buffer!");
}

void DeviceVK::submitToTransferQueue(const VkSubmitInfo& submitInfo, const VkFence fence) const
{
	if (vkQueueSubmit(m_TransferQueue, 1, &submitInfo, fence) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to submit command buffer!");
}

SwapChainSupportDetails DeviceVK::querySwapChainSupport()
{
	return querySwapChainSupport(m_PhysicalDevice);
}

QueueFamilyIndices DeviceVK::findQueueFamilies()
{
	return findQueueFamilies(m_PhysicalDevice);
}

VkImageView DeviceVK::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32_t layers)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image								= image;
	viewInfo.viewType							= viewType;
	viewInfo.format								= format;
	viewInfo.subresourceRange.aspectMask		= aspectFlags;
	viewInfo.subresourceRange.baseMipLevel		= 0;
	viewInfo.subresourceRange.levelCount		= 1;
	viewInfo.subresourceRange.baseArrayLayer	= 0;
	viewInfo.subresourceRange.layerCount		= layers;

	VkImageView imageView;
	if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture image view!");

	return imageView;
}

void DeviceVK::createImage(
	uint32_t width,
	uint32_t height,
	uint32_t layers,
	VkImageCreateFlags flags,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType				= VK_IMAGE_TYPE_2D;
	imageInfo.flags					= flags;
	imageInfo.extent.width			= width;
	imageInfo.extent.height			= height;
	imageInfo.extent.depth			= 1;
	imageInfo.mipLevels				= 1;
	imageInfo.arrayLayers			= layers;
	imageInfo.format				= format;
	imageInfo.tiling				= tiling;
	imageInfo.initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage					= usage;
	imageInfo.samples				= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize	= memRequirements.size;
	allocInfo.memoryTypeIndex	= findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to allocate image memory!");

	vkBindImageMemory(m_Device, image, imageMemory, 0);
}

void DeviceVK::createInstance(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extensions)
{
	VkApplicationInfo appInfo	= {};
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName	= "Vulkan";
	appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName			= "No Engine";
	appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo		= {};
	createInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo			= &appInfo;
	createInfo.enabledExtensionCount	= static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames	= extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	if (m_UseValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create VkInstance!");
}

void DeviceVK::setupValidationLayer()
{
	if (!m_UseValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerEXT(&createInfo) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to set up debug messenger!");
}

void DeviceVK::pickPhysicalDevice(const std::vector<const char*>& extensions)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("Error: Failed to find a GPU with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device, extensions))
		{
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Error: Failed to find a suitable GPU!");
}

void DeviceVK::createLogicalDevice(const std::vector<const char*>& validationLayers, const std::vector<const char*>& extensions)
{
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value(), indices.m_TransferFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= queueFamily;
		queueCreateInfo.queueCount			= 1;
		queueCreateInfo.pQueuePriorities	= &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount		= static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos		= queueCreateInfos.data();
	createInfo.pEnabledFeatures			= &deviceFeatures;
	createInfo.enabledExtensionCount	= static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames	= extensions.data();
	createInfo.enabledLayerCount = 0;

	if (m_UseValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");

	vkGetDeviceQueue(m_Device, indices.m_GraphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, indices.m_PresentFamily.value(), 0, &m_PresentQueue);
	vkGetDeviceQueue(m_Device, indices.m_TransferFamily.value(), 0, &m_TransferQueue);
}

void DeviceVK::createDescriptorPool()
{
	int descriptorCount = 1024;

	std::array<VkDescriptorPoolSize, 3> poolSizes = {};
	poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount	= descriptorCount;
	poolSizes[1].type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount	= descriptorCount;
	poolSizes[2].type				= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[2].descriptorCount	= descriptorCount;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount	= static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes		= poolSizes.data();
	poolInfo.maxSets		= descriptorCount;

	if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create descriptor pool!");
}

bool DeviceVK::isDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& extensions)
{
	if (!findQueueFamilies(device).isComplete())
		return false;

	if (!checkDeviceExtensionSupport(device, extensions))
		return false;

	if (!querySwapChainSupport(device).isComplete())
		return false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices DeviceVK::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size() && !indices.isComplete(); i++)
	{
		const auto& queueFamily = queueFamilies[i];
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.m_GraphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

			if (presentSupport)
				indices.m_PresentFamily = i;
		}
		else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			indices.m_TransferFamily = i;
		}
	}

	return indices;
}

bool DeviceVK::checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	for (const char* string : extensions)
	{
		bool found = false;
		for (const auto& extension : availableExtensions)
		{
			if (strcmp(string, extension.extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}

SwapChainSupportDetails DeviceVK::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.m_Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (formatCount != 0)
	{
		details.m_Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.m_Formats.data());
	}

	if (presentModeCount != 0)
	{
		details.m_PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.m_PresentModes.data());
	}

	return details;
}

uint32_t DeviceVK::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Error: Failed to find suitable memory type!");
}

VkInstance DeviceVK::getInstance() const
{
	return m_Instance;
}

void DeviceVK::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity	= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback	= debugCallback;
}

VkResult DeviceVK::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(m_Instance, pCreateInfo, nullptr, &m_DebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DeviceVK::destroyDebugUtilsMessengerEXT()
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(m_Instance, m_DebugMessenger, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DeviceVK::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}