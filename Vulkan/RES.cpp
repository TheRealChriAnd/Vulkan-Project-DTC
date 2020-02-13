#include "RES.h"

TextureVK* RES::TEXTURE_TEST	= nullptr;
TextureVK* RES::TEXTURE_FATBOY	= nullptr;

SamplerVK* RES::SAMPLER_DEFAULT = nullptr;

Mesh* RES::MESH_PLANE			= nullptr;
Mesh* RES::MESH_SOFA			= nullptr;
Mesh* RES::MESH_WALL1			= nullptr;
Mesh* RES::MESH_WALL2			= nullptr;
Mesh* RES::MESH_WINDOW			= nullptr;
Mesh* RES::MESH_TV				= nullptr;

void RES::init(DeviceVK* device)
{
	TEXTURE_TEST	= new TextureVK(device, "textures/test.png");
	TEXTURE_FATBOY	= new TextureVK(device, "textures/fatboy.png");

	SAMPLER_DEFAULT = new SamplerVK(device);

	MESH_PLANE		= Mesh::createPlane(device);
	MESH_SOFA		= Mesh::fromOBJ(device, "models/sofa4.obj");

	MESH_WALL1		= Mesh::fromOBJ(device, "models/wall.obj");
	MESH_WALL2		= Mesh::fromOBJ(device, "models/thinwall.obj");

	MESH_WINDOW		= Mesh::fromOBJ(device, "models/window.obj");
	MESH_TV			= Mesh::fromOBJ(device, "models/tv.obj");
}

void RES::shutdown()
{
	delete TEXTURE_TEST;
	delete TEXTURE_FATBOY;

	delete SAMPLER_DEFAULT;

	delete MESH_PLANE;
	delete MESH_SOFA;
	delete MESH_WALL1;
	delete MESH_WALL2;
	delete MESH_WINDOW;
	delete MESH_TV;
}
