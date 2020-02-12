#pragma once
#include <vector>
#include <glm/glm.hpp>

class DeviceVK;
class StorageBufferVK;
class IndexBufferVK;
class DescriptorSetVK;

class MeshVK
{
	friend class GameObject;
public:
	IndexBufferVK* m_IndexBuffer;

public:
	MeshVK(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec2>& uvs);
	MeshVK(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec2>& uvs, std::vector<uint16_t> indices);
	~MeshVK();

	static MeshVK* createPlane(DeviceVK* device);
	static MeshVK* createCube(DeviceVK* device);
private:
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferUV;
	
};