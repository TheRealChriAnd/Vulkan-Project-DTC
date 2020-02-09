#pragma once
#include "Defines.h"
#include "glm/glm.hpp"
#include <vector>
#include <set>
#include <string>

class WindowVK;
class DeviceVK;
class SwapChainVK;
class RenderPassVK;
class DescriptorSetLayoutVK;
class PipelineVK;
class TextureVK;
class SamplerVK;
class IndexBufferVK;
class StorageBufferVK;
class UniformBufferVK;
class DescriptorSetVK;
class CommandBufferVK;
class ShaderVK;
class RenderStateVK;
class MeshVK;

class RendererVK
{
public:
	RendererVK();
	~RendererVK();

	virtual MeshVK* makeMesh();
	virtual StorageBufferVK* makeVertexBuffer(size_t size);
	virtual TextureVK* makeTexture2D();
	virtual SamplerVK* makeSampler2D();
	virtual std::string getShaderPath();
	virtual std::string getShaderExtension();
	virtual UniformBufferVK* makeConstantBuffer(std::string NAME, unsigned int location);

	virtual int initialize(unsigned int width = 800, unsigned int height = 600);
	virtual void setWinTitle(const char* title);
	virtual void present();
	virtual int shutdown();

	virtual void setClearColor(float, float, float, float);
	virtual void clearBuffer(unsigned int);
	// can be partially overriden by a specific Technique.
	// submit work (to render) to the renderer.
	virtual void submit(MeshVK* mesh);
	virtual void frame();

	void createCommandBuffers(PipelineVK* pipeline);

	void createSyncObjects();

	void drawFrame();
	
private:
	WindowVK* m_Window;
	DeviceVK* m_Device;
	SwapChainVK* m_SwapChain;
	RenderPassVK* m_RenderPass;

	CommandBufferVK* m_CommandBuffer;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	std::vector<VkFence> m_ImagesInFlight;

	std::vector<MeshVK*> m_RenderQueue;
	std::vector<DescriptorSetLayoutVK*> m_DescriptorSetLayouts;
	glm::vec4 m_ColorClear;
	bool m_FirstFrame;
	std::set<PipelineVK*> m_Pipelines;
	std::vector<RenderStateVK*> m_RenderStates;
	std::vector<UniformBufferVK*> m_UniformBuffers;
	std::vector<DescriptorSetVK*> m_DescriptorSets;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};

