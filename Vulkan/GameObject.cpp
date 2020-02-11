#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include "MeshVK.h"
#include "DeviceVK.h"
#include "DescriptorSetVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "SwapChainVK.h"
#include "UniformBufferVK.h"
#include "DescriptorSetLayoutVK.h"

SamplerVK* GameObject::m_SamplerDefault = nullptr;
DeviceVK* GameObject::m_Device = nullptr;
SwapChainVK* GameObject::m_SwapChain = nullptr;
DescriptorSetLayoutVK* GameObject::m_Layout = nullptr;

GameObject::GameObject(DescriptorSetLayoutVK* layout, MeshVK* mesh, TextureVK* texture, SamplerVK* sampler)
{
	m_Mesh = mesh;
	m_Texture = texture;
	m_Sampler = sampler;

	m_Transform = glm::mat4(1.0F);

	m_UniformBuffer = new UniformBufferVK(m_Device, m_SwapChain, sizeof(glm::mat4));
	m_UniformBuffer->setData(&m_Transform);

	m_DescriptorSet = new DescriptorSetVK(m_Device, m_SwapChain, layout);
	m_DescriptorSet->addStorageBuffer(0, mesh->m_StorageBufferPos, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addStorageBuffer(1, mesh->m_StorageBufferUV, VK_WHOLE_SIZE, 0);
	m_DescriptorSet->addUniformBuffer(2, m_UniformBuffer);
	m_DescriptorSet->addTexture(3, texture, sampler);
	m_DescriptorSet->submit();
}

GameObject::~GameObject()
{
	delete m_UniformBuffer;
	delete m_DescriptorSet;
}

void GameObject::translate(const glm::vec3& offset)
{
	m_Transform = glm::translate(m_Transform, offset);
}

void GameObject::rotate(float angle, const glm::vec3& axle)
{
	m_Transform = glm::rotate(m_Transform, angle, axle);
}

void GameObject::scale(float scale)
{
	m_Transform = glm::scale(m_Transform, glm::vec3(scale, scale, scale));
}

void GameObject::applyTransform()
{
	m_UniformBuffer->setData(&m_Transform);
}

void GameObject::init(DeviceVK* device, SwapChainVK* swapChain)
{
	m_Device = device;
	m_SwapChain = swapChain;
	m_SamplerDefault = new SamplerVK(device);
}

void GameObject::shutdown()
{
	delete m_SamplerDefault;
}
