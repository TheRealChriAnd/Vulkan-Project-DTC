#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtx/hash.hpp>

class DeviceVK;
class StorageBufferVK;
class IndexBufferVK;

struct Vertex
{
	glm::vec4 pos;
	glm::vec4 nor;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && nor == other.nor && texCoord == other.texCoord;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec4>()(vertex.pos) ^ (hash<glm::vec4>()(vertex.nor) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

class Mesh
{
protected:
	Mesh(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec4>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint16_t>& indices);
	
public:
	~Mesh();

	StorageBufferVK* getStorageBufferPos() const;
	StorageBufferVK* getStorageBufferNor() const;
	StorageBufferVK* getStorageBufferUV() const;
	IndexBufferVK* getIndexBuffer() const;

	static Mesh* createPlane(DeviceVK* device);
	static Mesh* createCube(DeviceVK* device);
	static Mesh* fromOBJ(DeviceVK* device, const std::string& file);

private:
	StorageBufferVK* m_StorageBufferPos;
	StorageBufferVK* m_StorageBufferNor;
	StorageBufferVK* m_StorageBufferUV;
	IndexBufferVK* m_IndexBuffer;
};