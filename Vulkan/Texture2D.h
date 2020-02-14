#pragma once
#include "Defines.h"
#include "TextureVK.h"
#include <string>

class BufferVK;

class Texture2D : public TextureVK
{
public:
	Texture2D(DeviceVK* device, const std::string& file);
	virtual ~Texture2D();

private:
	void createTextureImage(DeviceVK* device, const std::string& file);

private:

};
