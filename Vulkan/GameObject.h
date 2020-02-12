#pragma once
#include <vector>
#include <glm/glm.hpp>

class Mesh;
class DescriptorSetVK;
class DeviceVK;
class SwapChainVK;
class SamplerVK;
class UniformBufferVK;

class GameObject
{
public:
	GameObject(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer);
	virtual ~GameObject();

	Mesh* getMesh() const;
	DescriptorSetVK* getDescriptorSet() const;

protected:
	Mesh* m_Mesh;
	UniformBufferVK* m_UniformBuffer;
	DescriptorSetVK* m_DescriptorSet;
};