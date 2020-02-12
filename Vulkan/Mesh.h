#pragma once
#include <vector>
#include <glm/glm.hpp>

class DeviceVK;
class StorageBufferVK;
class IndexBufferVK;

class Mesh
{
public:
	Mesh(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec4>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint16_t>& indices);
	~Mesh();

	StorageBufferVK* getStorageBufferPos() const;
	StorageBufferVK* getStorageBufferNor() const;
	StorageBufferVK* getStorageBufferUV() const;
	IndexBufferVK* getIndexBuffer() const;

	static Mesh* createPlane(DeviceVK* device);
	static Mesh* createCube(DeviceVK* device);

private:
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferNor;
	StorageBufferVK* m_StorageBufferUV;
	IndexBufferVK* m_IndexBuffer;
};