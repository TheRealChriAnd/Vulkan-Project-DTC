#pragma once

#include "Texture2D.h"
#include "TextureSkyBox.h"
#include "TextureAnimated.h"
#include "SamplerVK.h"
#include "Mesh.h"

class DeviceVK;

class RES
{
	friend class Application;
public:
	static Texture2D* TEXTURE_THIN;
	static Texture2D* TEXTURE_SOFA;
	static Texture2D* TEXTURE_TV;
	static Texture2D* TEXTURE_FLOOR;
	static Texture2D* TEXTURE_GROUND;
	static Texture2D* TEXTURE_TABLE;

	static TextureSkyBox* TEXTURE_SKYBOX;

	static TextureAnimated* TEXTURE_ANIMATED;

	static SamplerVK* SAMPLER_DEFAULT;
	
	static Mesh* MESH_PLANE;
	static Mesh* MESH_SOFA;
	static Mesh* MESH_WALL2;
	static Mesh* MESH_WINDOW;
	static Mesh* MESH_TV;
	static Mesh* MESH_CUBE;
	static Mesh* MESH_TABLE;

private:
	static void init(DeviceVK* device);
	static void shutdown();
};