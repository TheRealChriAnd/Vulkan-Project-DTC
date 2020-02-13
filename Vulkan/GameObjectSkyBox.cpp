#include "GameObjectSkyBox.h"

#include "DescriptorSetVK.h"
#include "Mesh.h"
#include "UniformBufferVK.h"

GameObjectSkyBox::GameObjectSkyBox(DescriptorSetVK* descriptorSet, Mesh* mesh, UniformBufferVK* uniformBuffer, TextureSkyBox* texture) :
	GameObject(descriptorSet, mesh, uniformBuffer)
{
	m_Texture = texture;
}

GameObjectSkyBox::~GameObjectSkyBox()
{
}