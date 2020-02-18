#include "IRenderer.h"
#include "DeviceVK.h"
#include "SwapChainVK.h"
#include "PipelineVK.h"
#include "LightVK.h"

IRenderer::IRenderer(DeviceVK* device, SwapChainVK* swapChain, RenderPassVK* renderPass)
{
	m_Device = device;
	m_SwapChain = swapChain;
	m_RenderPass = renderPass;
}

IRenderer::~IRenderer()
{

}

void IRenderer::addLight(LightVK* light) 
{
	auto iterator = m_Lights.find(light->m_LightType);
	if (iterator != m_Lights.end())
	{
		iterator->second.push_back(light);
	}
	else
	{
		m_Lights.insert({ light->m_LightType, {light} });
	}
}
