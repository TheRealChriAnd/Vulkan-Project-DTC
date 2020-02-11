#pragma once
#include <vector>
#include <glm/glm.hpp>

class DeviceVK;
class StorageBufferVK;
class DescriptorSetVK;

class MeshVK
{
	friend class GameObject;
public:
	MeshVK(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec2>& uvs);
	~MeshVK();

private:
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferUV;
};