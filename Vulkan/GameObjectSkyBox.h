#pragma once
#include "GameObject.h"

class Mesh;
class UniformBufferVK;
class DescriptorSetVK;
class TextureSkyBox;

class GameObjectSkyBox : public GameObject
{
public:
	GameObjectSkyBox(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureSkyBox* texture);
	virtual ~GameObjectSkyBox();

private:
	TextureSkyBox* m_Texture;
};