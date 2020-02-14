#include "RES.h"

TextureVK* RES::TEXTURE_TEST	= nullptr;
TextureVK* RES::TEXTURE_FATBOY	= nullptr;
TextureVK* RES::TEXTURE_WALL	= nullptr;
TextureVK* RES::TEXTURE_THIN	= nullptr;
TextureVK* RES::TEXTURE_SOFA	= nullptr;
TextureVK* RES::TEXTURE_TV		= nullptr;

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
	MESH_SOFA		= Mesh::fromOBJ(device, "models/sofa.obj");
	TEXTURE_SOFA	= new TextureVK(device, "textures/cloth_dif.jpg");

	MESH_WALL1		= Mesh::fromOBJ(device, "models/wall.obj");
	MESH_WALL2		= Mesh::fromOBJ(device, "models/thinwall.obj");
	TEXTURE_WALL	= new TextureVK(device, "textures/wall/plywood_texture.jpg");
	TEXTURE_THIN	= new TextureVK(device, "textures/walltexture.jfif");

	MESH_WINDOW		= Mesh::fromOBJ(device, "models/window.obj");
	MESH_TV			= Mesh::fromOBJ(device, "models/tv.obj");
	TEXTURE_TV	= new TextureVK(device, "textures/blackgloss.jfif");
}

void RES::shutdown()
{
	delete TEXTURE_TEST;
	delete TEXTURE_FATBOY;
	delete TEXTURE_WALL;
	delete TEXTURE_THIN;
	delete TEXTURE_SOFA;
	delete TEXTURE_TV;

	delete SAMPLER_DEFAULT;

	delete MESH_PLANE;
	delete MESH_SOFA;
	delete MESH_WALL1;
	delete MESH_WALL2;
	delete MESH_WINDOW;
	delete MESH_TV;
}
