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
	static Texture2D* TEXTURE_TEST;
	static Texture2D* TEXTURE_FATBOY;

	static TextureSkyBox* TEXTURE_SKYBOX;

	static SamplerVK* SAMPLER_DEFAULT;
	
	static Mesh* MESH_PLANE;
	static Mesh* MESH_CUBE;
	static Mesh* MESH_TEST;

private:
	static void init(DeviceVK* device);
	static void shutdown();
};