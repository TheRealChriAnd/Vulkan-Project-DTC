#pragma once
#include <vector>
#include <glm/glm.hpp>

class MeshVK;
class DescriptorSetVK;
class TextureVK;
class SamplerVK;
class DeviceVK;
class SwapChainVK;
class DescriptorSetLayoutVK;
class UniformBufferVK;

class GameObject
{
	friend class Application;
public:
	GameObject(DescriptorSetLayoutVK* layout, MeshVK* mesh, TextureVK* m_Texture, SamplerVK* m_Sampler = m_SamplerDefault);
	~GameObject();

	void translate(const glm::vec3& offset);
	void rotate(float angle, const glm::vec3& axle);
	void scale(float scale);

	void applyTransform();

private:
	static void init(DeviceVK* device, SwapChainVK* swapChain);
	static void shutdown();

private:
	MeshVK* m_Mesh;
	TextureVK* m_Texture;
	SamplerVK* m_Sampler;
	UniformBufferVK* m_UniformBuffer;
	DescriptorSetVK* m_DescriptorSet;
	glm::mat4 m_Transform;

	static SamplerVK* m_SamplerDefault;
	static DeviceVK* m_Device;
	static SwapChainVK* m_SwapChain;
	static DescriptorSetLayoutVK* m_Layout;
};