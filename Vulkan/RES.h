#pragma once

#include "Texture2D.h"
#include "TextureSkyBox.h"
#include "SamplerVK.h"
#include "Mesh.h"

class DeviceVK;

class RES
{
	friend class Application;
public:
	static TextureVK* TEXTURE_WALL;
	static TextureVK* TEXTURE_THIN;
	static TextureVK* TEXTURE_SOFA;
	static TextureVK* TEXTURE_TV;
	static Texture2D* TEXTURE_TEST;
	static Texture2D* TEXTURE_FATBOY;

	static TextureSkyBox* TEXTURE_SKYBOX;

	static SamplerVK* SAMPLER_DEFAULT;
	
	static Mesh* MESH_PLANE;
	static Mesh* MESH_SOFA;
	static Mesh* MESH_WALL1;
	static Mesh* MESH_WALL2;
	static Mesh* MESH_WINDOW;
	static Mesh* MESH_TV;
	static Mesh* MESH_CUBE;
	static Mesh* MESH_TEST;

private:
	static void init(DeviceVK* device);
	static void shutdown();
};