#include "Mesh.h"
#include "DeviceVK.h"
#include "StorageBufferVK.h"
#include "DescriptorSetVK.h"
#include "IndexBufferVK.h"

Mesh::Mesh(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec4>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint16_t>& indices)
{
	m_StorageBufferPos = new StorageBufferVK(device);
	m_StorageBufferPos->setData(positions.data(), positions.size() * sizeof(glm::vec4), 0);

	m_StorageBufferNor = new StorageBufferVK(device);
	m_StorageBufferNor->setData(normals.data(), normals.size() * sizeof(glm::vec4), 0);

	m_StorageBufferUV = new StorageBufferVK(device);
	m_StorageBufferUV->setData(uvs.data(), uvs.size() * sizeof(glm::vec2), 0);

	m_IndexBuffer = new IndexBufferVK(device, indices);
}

Mesh::~Mesh()
{
	delete m_StorageBufferPos;
	delete m_StorageBufferNor;
	delete m_StorageBufferUV;
	delete m_IndexBuffer;
}

StorageBufferVK* Mesh::getStorageBufferPos() const
{
	return m_StorageBufferPos;
}

StorageBufferVK* Mesh::getStorageBufferNor() const
{
	return m_StorageBufferNor;
}

StorageBufferVK* Mesh::getStorageBufferUV() const
{
	return m_StorageBufferUV;
}

IndexBufferVK* Mesh::getIndexBuffer() const
{
	return m_IndexBuffer;
}

Mesh* Mesh::createPlane(DeviceVK* device)
{
	static const std::vector<glm::vec4> pos =
	{
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f}
	};

	static const std::vector<glm::vec4> nor =
	{
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f}
	};

	static const std::vector<glm::vec2> uv =
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}
	};

	static const std::vector<uint16_t> indices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	return new Mesh(device, pos, nor, uv, indices);
}

Mesh* Mesh::createCube(DeviceVK* device)
{
	static const std::vector<glm::vec4>  pos =
	{
		{-1.0f,-1.0f,-1.0f, 1.0f},  // -X side
		{-1.0f,-1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f,-1.0f, 1.0f},
		{-1.0f,-1.0f,-1.0f, 1.0f},

		{-1.0f,-1.0f,-1.0f, 1.0f},  // -Z side
		{ 1.0f, 1.0f,-1.0f, 1.0f},
		{ 1.0f,-1.0f,-1.0f, 1.0f},
		{-1.0f,-1.0f,-1.0f, 1.0f},
		{-1.0f, 1.0f,-1.0f, 1.0f},
		{ 1.0f, 1.0f,-1.0f, 1.0f},

		{-1.0f,-1.0f,-1.0f, 1.0f},  // -Y side
		{ 1.0f,-1.0f,-1.0f, 1.0f},
		{ 1.0f,-1.0f, 1.0f, 1.0f},
		{-1.0f,-1.0f,-1.0f, 1.0f},
		{ 1.0f,-1.0f, 1.0f, 1.0f},
		{-1.0f,-1.0f, 1.0f, 1.0f},

		{-1.0f, 1.0f,-1.0f, 1.0f},  // +Y side
		{-1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f,-1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f,-1.0f, 1.0f},

		{ 1.0f, 1.0f,-1.0f, 1.0f},  // +X side
		{ 1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f,-1.0f, 1.0f, 1.0f},
		{ 1.0f,-1.0f, 1.0f, 1.0f},
		{ 1.0f,-1.0f,-1.0f, 1.0f},
		{ 1.0f, 1.0f,-1.0f, 1.0f},

		{-1.0f, 1.0f, 1.0f, 1.0f},  // +Z side
		{-1.0f,-1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f,-1.0f, 1.0f, 1.0f},
		{ 1.0f,-1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f, 1.0f}
	};

	// Add normals here.......................................
	static const std::vector<glm::vec4> nor =
	{
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f}
	};

	static const std::vector<glm::vec2> uv =
	{
		{0.0f, 1.0f},  // -X side
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},

		{1.0f, 1.0f},  // -Z side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{1.0f, 0.0f},  // -Y side
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},

		{1.0f, 0.0f},  // +Y side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},

		{1.0f, 0.0f},  // +X side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},

		{0.0f, 0.0f},  // +Z side
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f}
	};

	static const std::vector<uint16_t> indices =
	{
		0, 1, 2, 0, 2, 3,    // front
		4, 5, 6, 4, 6, 7,    // back
		8, 9, 10, 8, 10, 11,   // top
		12, 13, 14, 12, 14, 15,   // bottom
		16, 17, 18, 16, 18, 19,   // right
		20, 21, 22, 20, 22, 23   // left
	};

	return new Mesh(device, pos, nor, uv, indices);
}
