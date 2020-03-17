#pragma once
#include "GameObjectSimple.h"

class TextureAnimated;

class GameObjectAnimated : public GameObjectSimple
{
public:
	GameObjectAnimated(DescriptorSetVK* descriptorSet1, DescriptorSetVK* descriptorSet2, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureAnimated* texture, SamplerVK* sampler);
	virtual ~GameObjectAnimated();

	virtual DescriptorSetVK* getDescriptorSet() const override;

private:
	DescriptorSetVK* m_DescriptorSet2;
};

