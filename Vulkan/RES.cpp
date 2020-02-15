#include "RES.h"

Texture2D* RES::TEXTURE_FLOOR	= nullptr;
Texture2D* RES::TEXTURE_THIN	= nullptr;
Texture2D* RES::TEXTURE_SOFA	= nullptr;
Texture2D* RES::TEXTURE_TV		= nullptr;
Texture2D* RES::TEXTURE_GROUND  = nullptr;
Texture2D* RES::TEXTURE_TABLE	= nullptr;

TextureSkyBox* RES::TEXTURE_SKYBOX = nullptr;

TextureAnimated* RES::TEXTURE_ANIMATED = nullptr;

SamplerVK* RES::SAMPLER_DEFAULT = nullptr;

Mesh* RES::MESH_PLANE			= nullptr;

Mesh* RES::MESH_SOFA			= nullptr;
Mesh* RES::MESH_WALL2			= nullptr;
Mesh* RES::MESH_WINDOW			= nullptr;
Mesh* RES::MESH_TV				= nullptr;
Mesh* RES::MESH_TABLE			= nullptr;

Mesh* RES::MESH_CUBE			= nullptr;

void RES::init(DeviceVK* device)
{
	TEXTURE_FLOOR = new Texture2D(device, "textures/floor.jfif");
	TEXTURE_GROUND = new Texture2D(device, "textures/ground.png");

	TEXTURE_SKYBOX = new TextureSkyBox(device);

	TextureLayers texLayers;
	texLayers.fileNames[0] = "textures/clouds/east.bmp";
	texLayers.fileNames[1] = "textures/clouds/west.bmp";
	texLayers.fileNames[2] = "textures/clouds/up.bmp";
	texLayers.fileNames[3] = "textures/clouds/down.bmp";
	texLayers.fileNames[4] = "textures/clouds/north.bmp";
	texLayers.fileNames[5] = "textures/clouds/south.bmp";

	TEXTURE_SKYBOX->loadFromFile(texLayers);

	TEXTURE_ANIMATED = new TextureAnimated(device, "videos/video.mp4");

	SAMPLER_DEFAULT = new SamplerVK(device);

	MESH_PLANE		= Mesh::createPlane(device);

	MESH_SOFA		= Mesh::fromOBJ(device, "models/sofa.obj");
	TEXTURE_SOFA	= new Texture2D(device, "textures/sofa_cloth.jpg");

	MESH_WALL2		= Mesh::fromOBJ(device, "models/thinwall.obj");
	TEXTURE_THIN	= new Texture2D(device, "textures/walltexture.jpg");

	MESH_WINDOW		= Mesh::fromOBJ(device, "models/window.obj");
	MESH_TV			= Mesh::fromOBJ(device, "models/tv.obj");
	TEXTURE_TV		= new Texture2D(device, "textures/blackgloss.jfif");

	MESH_TABLE		= Mesh::fromOBJ(device, "models/coffeetable.obj");
	TEXTURE_TABLE	= new Texture2D(device, "textures/wood_bump.png");

	MESH_CUBE		= Mesh::createCube(device);
}

void RES::shutdown()
{
	delete TEXTURE_FLOOR;
	delete TEXTURE_THIN;
	delete TEXTURE_SOFA;
	delete TEXTURE_TV;
	delete TEXTURE_GROUND;
	delete TEXTURE_TABLE;

	delete TEXTURE_SKYBOX;

	delete TEXTURE_ANIMATED;

	delete SAMPLER_DEFAULT;

	delete MESH_PLANE;

	delete MESH_SOFA;
	delete MESH_WALL2;
	delete MESH_WINDOW;
	delete MESH_TV;

	delete MESH_CUBE;
}
