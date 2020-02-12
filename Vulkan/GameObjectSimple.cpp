#include "GameObjectSimple.h"
#include <glm/gtc/matrix_transform.hpp>

#include "DescriptorSetVK.h"
#include "Mesh.h"
#include "UniformBufferVK.h"
#include "TextureVK.h"
#include "SamplerVK.h"

GameObjectSimple::GameObjectSimple(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureVK* m_Texture, SamplerVK* m_Sampler) : 
	GameObject(descriptorSet, mesh, uniformBuffer)
{
	m_Transform = glm::mat4(1.0F);
}

GameObjectSimple::~GameObjectSimple()
{
}

void GameObjectSimple::translate(const glm::vec3& offset)
{
	m_Transform = glm::translate(m_Transform, offset);
}

void GameObjectSimple::rotate(float angle, const glm::vec3& axle)
{
	m_Transform = glm::rotate(m_Transform, angle, axle);
}

void GameObjectSimple::scale(float scale)
{
	m_Transform = glm::scale(m_Transform, glm::vec3(scale, scale, scale));
}

void GameObjectSimple::applyTransform()
{
	m_UniformBuffer->setData(&m_Transform);
}