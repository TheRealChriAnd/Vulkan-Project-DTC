#include "RES.h"

TextureVK* RES::TEXTURE_TEST	= nullptr;
TextureVK* RES::TEXTURE_FATBOY	= nullptr;

SamplerVK* RES::SAMPLER_DEFAULT = nullptr;

Mesh* RES::MESH_PLANE			= nullptr;
Mesh* RES::MESH_TEST			= nullptr;

void RES::init(DeviceVK* device)
{
	TEXTURE_TEST	= new TextureVK(device, "textures/test.png");
	TEXTURE_FATBOY	= new TextureVK(device, "textures/fatboy.png");

	SAMPLER_DEFAULT = new SamplerVK(device);

	MESH_PLANE		= Mesh::createPlane(device);
	MESH_TEST		= Mesh::fromOBJ(device, "models/box_stack.obj");
}

void RES::shutdown()
{
	delete TEXTURE_TEST;
	delete TEXTURE_FATBOY;

	delete SAMPLER_DEFAULT;

	delete MESH_PLANE;
	delete MESH_TEST;
}
