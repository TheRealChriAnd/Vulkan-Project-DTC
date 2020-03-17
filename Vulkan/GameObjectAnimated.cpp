#include "GameObjectAnimated.h"

#include "DescriptorSetVK.h"
#include "Mesh.h"
#include "UniformBufferVK.h"
#include "TextureAnimated.h"
#include "SamplerVK.h"

GameObjectAnimated::GameObjectAnimated(DescriptorSetVK* descriptorSet1, DescriptorSetVK* descriptorSet2, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureAnimated* texture, SamplerVK* sampler) : 
	GameObjectSimple(descriptorSet1, mesh, uniformBuffer, texture, sampler)
{
	m_DescriptorSet2 = descriptorSet2;
}

GameObjectAnimated::~GameObjectAnimated()
{

}

DescriptorSetVK* GameObjectAnimated::getDescriptorSet() const
{
	TextureAnimated* texture = (TextureAnimated*)m_Texture;
	return texture->useFirstImage() ? m_DescriptorSet : m_DescriptorSet2;
}
