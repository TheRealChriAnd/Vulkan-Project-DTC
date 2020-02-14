#include "RES.h"

Texture2D* RES::TEXTURE_TEST	= nullptr;
Texture2D* RES::TEXTURE_FATBOY	= nullptr;

TextureSkyBox* RES::TEXTURE_SKYBOX = nullptr;

TextureAnimated* RES::TEXTURE_ANIMATED = nullptr;

SamplerVK* RES::SAMPLER_DEFAULT = nullptr;

Mesh* RES::MESH_PLANE			= nullptr;
Mesh* RES::MESH_CUBE			= nullptr;
Mesh* RES::MESH_TEST			= nullptr;

void RES::init(DeviceVK* device)
{
	TEXTURE_TEST	= new Texture2D(device, "textures/test.png");
	TEXTURE_FATBOY	= new Texture2D(device, "textures/fatboy.png");

	TEXTURE_SKYBOX = new TextureSkyBox(device);

	TextureLayers texLayers;
	texLayers.fileNames[0] = "textures/clouds/east.bmp";
	texLayers.fileNames[1] = "textures/clouds/west.bmp";
	texLayers.fileNames[2] = "textures/clouds/up.bmp";
	texLayers.fileNames[3] = "textures/clouds/down.bmp";
	texLayers.fileNames[4] = "textures/clouds/north.bmp";
	texLayers.fileNames[5] = "textures/clouds/south.bmp";

	TEXTURE_SKYBOX->loadFromFile(texLayers);

	TEXTURE_ANIMATED = new TextureAnimated(device, "videos/Toy Story 3.mp4");

	SAMPLER_DEFAULT = new SamplerVK(device);

	MESH_PLANE		= Mesh::createPlane(device);
	MESH_CUBE		= Mesh::createCube(device);
	MESH_TEST		= Mesh::fromOBJ(device, "models/box_stack.obj");
}

void RES::shutdown()
{
	delete TEXTURE_TEST;
	delete TEXTURE_FATBOY;

	delete TEXTURE_SKYBOX;

	delete TEXTURE_ANIMATED;

	delete SAMPLER_DEFAULT;

	delete MESH_PLANE;
	delete MESH_CUBE;
	delete MESH_TEST;
}
