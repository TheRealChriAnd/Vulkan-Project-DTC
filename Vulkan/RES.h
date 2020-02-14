#pragma once

#include "TextureVK.h"
#include "SamplerVK.h"
#include "Mesh.h"

class DeviceVK;

class RES
{
	friend class Application;
public:
	static TextureVK* TEXTURE_TEST;
	static TextureVK* TEXTURE_FATBOY;
	static TextureVK* TEXTURE_WALL;
	static TextureVK* TEXTURE_THIN;
	static TextureVK* TEXTURE_SOFA;
	static TextureVK* TEXTURE_TV;

	static SamplerVK* SAMPLER_DEFAULT;
	
	static Mesh* MESH_PLANE;
	static Mesh* MESH_SOFA;
	static Mesh* MESH_WALL1;
	static Mesh* MESH_WALL2;
	static Mesh* MESH_WINDOW;
	static Mesh* MESH_TV;

private:
	static void init(DeviceVK* device);
	static void shutdown();
};