#pragma once
#include "Defines.h"
#include <vector>

class WindowVK;
class DeviceVK;
class SwapChainVK;
class RenderPassVK;
class CommandBufferVK;

class Application
{
public:
	Application(int tvs);
	virtual ~Application() {};
	
	void run();

protected:
	virtual void preInit() = 0;
	virtual void init() = 0;
	virtual void update(float deltaSeconds) = 0;
	virtual CommandBufferVK* frame() = 0;
	virtual void shutdown() = 0;

	virtual void onSwapChainReleased() = 0;
	virtual void onSwapChainCreated() = 0;

private:
	void shutdownInternal();
	void createSwapChainInternal();
	void releaseSwapChainInternal();
	void recreateSwapChain();
	void createSyncObjects();
	void drawFrame(uint32_t imageIndex);

protected:
	WindowVK* m_Window;
	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
	RenderPassVK* m_RenderPass;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	std::vector<VkFence> m_ImagesInFlight;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;
	int m_NrOfTVs;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};
