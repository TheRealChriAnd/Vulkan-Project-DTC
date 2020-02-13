#pragma once
#include "GameObject.h"

class Mesh;
class TextureVK;
class SamplerVK;
class UniformBufferVK;
class DescriptorSetVK;

class GameObjectSimple : public GameObject
{
public:
	GameObjectSimple(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureVK* m_Texture, SamplerVK* m_Sampler);
	virtual ~GameObjectSimple();

	void translate(const glm::vec3& offset);
	void rotate(float angle, const glm::vec3& axle);
	void scale(float scale);

	void applyTransform();

private:
	TextureVK* m_Texture;
	SamplerVK* m_Sampler;

	glm::mat4 m_Transform;
};