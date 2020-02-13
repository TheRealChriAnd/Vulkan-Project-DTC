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

	static SamplerVK* SAMPLER_DEFAULT;
	
	static Mesh* MESH_PLANE;
	static Mesh* MESH_TEST;

private:
	static void init(DeviceVK* device);
	static void shutdown();
};