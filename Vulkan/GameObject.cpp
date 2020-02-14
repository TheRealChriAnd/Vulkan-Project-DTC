#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "DeviceVK.h"
#include "DescriptorSetVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"
#include "SwapChainVK.h"
#include "UniformBufferVK.h"
#include "DescriptorSetLayoutVK.h"

GameObject::GameObject(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer)
{
	m_Mesh = mesh;
	m_UniformBuffer = uniformBuffer;
	m_DescriptorSet = descriptorSet;
}

GameObject::~GameObject()
{
	delete m_UniformBuffer;
	delete m_DescriptorSet;
}

Mesh* GameObject::getMesh() const
{
	return m_Mesh;
}

DescriptorSetVK* GameObject::getDescriptorSet() const
{
	return m_DescriptorSet;
}
