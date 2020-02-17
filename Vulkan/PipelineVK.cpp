#include "PipelineVK.h"
#include "DeviceVK.h"
#include "RenderPassVK.h"
#include "DescriptorSetLayoutVK.h"
#include "SwapChainVK.h"
#include "ShaderVK.h"
#include <iostream>
#include <fstream>

#define INDEX_RAYGEN 0
#define INDEX_MISS 1
#define INDEX_CLOSEST_HIT 2

PipelineVK::PipelineVK(DeviceVK* device)
{
	m_Device = device;
}

PipelineVK::~PipelineVK()
{
	VkDevice device = m_Device->getDevice();
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
}

VkPipeline PipelineVK::getPipeline() const
{
	return m_Pipeline;
}

VkPipelineLayout PipelineVK::getLayout() const
{
	return m_PipelineLayout;
}

void PipelineVK::addShader(ShaderVK* shader)
{
	m_Shaders.push_back(shader->getShaderInfo());
}

void PipelineVK::addDescriptorLayout(DescriptorSetLayoutVK* descriptorSetLayout)
{
	m_Layouts.push_back(descriptorSetLayout->getDescriptorSetLayout());
}

void PipelineVK::submit(DeviceVK* device, RenderPassVK* renderPass, SwapChainVK* swapChain, VkPolygonMode polygonMode)
{
	createGraphicsPipeline(device, renderPass, swapChain, polygonMode);
}

void PipelineVK::createGraphicsPipeline(DeviceVK* device, RenderPassVK* renderPass, SwapChainVK* swapChain, VkPolygonMode polygonMode)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertexInputInfo.vertexBindingDescriptionCount	= 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions		= nullptr;
	vertexInputInfo.pVertexAttributeDescriptions	= nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable	= VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChain->getExtent().width;
	viewport.height = (float)swapChain->getExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain->getExtent();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = m_Layouts.size();
	pipelineLayoutInfo.pSetLayouts = m_Layouts.data();

	if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create pipeline layout!");

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(m_Shaders.size());
	pipelineInfo.pStages = m_Shaders.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = renderPass->getRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create graphics pipeline!");
}

void PipelineVK::createRayTracingPipeline(DeviceVK* device, RenderPassVK* renderPass, SwapChainVK* swapChain)
{
	VkPipelineLayoutCreateInfo pipeLineLayoutCreateInfo{};
	pipeLineLayoutCreateInfo.sType			= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLineLayoutCreateInfo.setLayoutCount = m_Layouts.size();
	pipeLineLayoutCreateInfo.pSetLayouts	= m_Layouts.data();

	if (vkCreatePipelineLayout(device->getDevice(), &pipeLineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create pipeline layout!");

	const uint32_t shaderIndexRaygen		= 0;
	const uint32_t shaderIndexMiss			= 1;
	const uint32_t shaderIndexClosestHit	= 2;

	std::array<VkRayTracingShaderGroupCreateInfoNV, 3> groups{};
	for (auto& group : groups)
	{
		group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV;
		group.generalShader			= VK_SHADER_UNUSED_NV;
		group.closestHitShader		= VK_SHADER_UNUSED_NV;
		group.anyHitShader			= VK_SHADER_UNUSED_NV;
		group.intersectionShader	= VK_SHADER_UNUSED_NV;
	}

	groups[INDEX_RAYGEN].type					= VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
	groups[INDEX_RAYGEN].generalShader			= shaderIndexRaygen;
	groups[INDEX_MISS].type						= VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
	groups[INDEX_MISS].generalShader			= shaderIndexMiss;
	groups[INDEX_CLOSEST_HIT].type				= VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV;
	groups[INDEX_CLOSEST_HIT].generalShader		= VK_SHADER_UNUSED_NV;
	groups[INDEX_CLOSEST_HIT].closestHitShader	= shaderIndexClosestHit;
	//------- dubbelkolla raden mellan

	VkRayTracingPipelineCreateInfoNV rayPipelineInfo{};
	rayPipelineInfo.sType				= VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV;
	rayPipelineInfo.stageCount			= static_cast<uint32_t>(m_Shaders.size());
	rayPipelineInfo.pStages				= m_Shaders.data();
	rayPipelineInfo.groupCount			= static_cast<uint32_t>(groups.size());
	rayPipelineInfo.pGroups				= groups.data();
	rayPipelineInfo.maxRecursionDepth	= 1;
	rayPipelineInfo.layout				= m_PipelineLayout;
	
	if (vkCreateRayTracingPipelinesNV(device->getDevice(), VK_NULL_HANDLE, 1, &rayPipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		throw std::runtime_error("Error: Failed to create raytracing pipeline!");

}