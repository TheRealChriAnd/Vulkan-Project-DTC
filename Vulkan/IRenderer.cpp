#include "IRenderer.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "PipelineVK.h"

IRenderer::IRenderer(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass)
{
	m_Device = device;
	m_SwapChain = swapChain;
	m_RenderPass = renderPass;
}

IRenderer::~IRenderer()
{

}