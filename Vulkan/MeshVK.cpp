#include "MeshVK.h"
#include "DeviceVK.h"
#include "StorageBufferVK.h"
#include "DescriptorSetVK.h"

MeshVK::MeshVK(DeviceVK* device, const std::vector<glm::vec4>& positions, const std::vector<glm::vec2>& uvs)
{
	m_StorageBufferPos = new StorageBufferVK(device);
	m_StorageBufferPos->setData(positions.data(), positions.size() * sizeof(glm::vec4), 0);

	m_StorageBufferUV = new StorageBufferVK(device);
	m_StorageBufferUV->setData(uvs.data(), uvs.size() * sizeof(glm::vec2), 0);
}

MeshVK::~MeshVK()
{
	
}