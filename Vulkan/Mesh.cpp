#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.h"
#include "DeviceVK.h"
#include "StorageBufferVK.h"
#include "DescriptorSetVK.h"
#include "IndexBufferVK.h"
#include "tiny_obj_loader.h"
#include <unordered_map>

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
		{-0.5f, 0.0f, -0.5f, 1.0f},
		{0.5f, 0.0f, -0.5f, 1.0f},
		{0.5f, 0.0f, 0.5f, 1.0f},
		{-0.5f, 0.0f, 0.5f, 1.0f}
	};

	static const std::vector<glm::vec4> nor =
	{
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f}
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
		{ -1.0f, 1.0f, -1.0f, 1.0F},
		{ 1.0f, 1.0f, -1.0f, 1.0F },
		{ -1.0f, -1.0f, -1.0f, 1.0F },   
		{ 1.0f, -1.0f, -1.0f, 1.0F},
		{ -1.0f, 1.0f, 1.0f, 1.0F },
		{ 1.0f, 1.0f, 1.0f, 1.0F },
		{ -1.0f, -1.0f, 1.0f, 1.0F },
		{ 1.0f, -1.0f, 1.0f, 1.0F }
	};

	// Add normals here.......................................
	static const std::vector<glm::vec4> nor =
	{
		{0.0f, 0.0f, 1.0f, 1.0f}
	};

	static const std::vector<glm::vec2> uv =
	{
		{0.0f, 1.0f}
	};

	static const std::vector<uint16_t> indices =
	{
		0, 1, 2,
		2, 1, 3,
		4, 0, 6,
		6, 0, 2,
		7, 5, 6,
		6, 5, 4,
		3, 1, 7,
		7, 1, 5,
		4, 5, 0,
		0, 5, 1,
		3, 7, 2,
		2, 7, 6,
	};

	return new Mesh(device, pos, nor, uv, indices);
}

Mesh* Mesh::fromOBJ(DeviceVK* device, const std::string& file)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	std::vector<glm::vec4> positions;
	std::vector<glm::vec4> normals;
	std::vector<glm::vec2> uvs;
	std::vector<uint16_t> indices;

	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2], 
				1.0F
			};

			if (index.normal_index >= 0)
			{
				vertex.nor =
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
					1.0F
				};
			}

			vertex.texCoord =
			{
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(positions.size());
				positions.push_back(vertex.pos);
				normals.push_back(vertex.nor);
				uvs.push_back(vertex.texCoord);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	return new Mesh(device, positions, normals, uvs, indices);
}
