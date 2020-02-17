#include "AccelerationStructure.h"
#include "DeviceVK.h"

AccelerationStructure::AccelerationStructure(DeviceVK* device)
{
	m_Device = device;
}

AccelerationStructure::~AccelerationStructure()
{
}

void AccelerationStructure::createBottomGeometry(uint32_t indexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer)
{
	VkGeometryNV geometry{};
	geometry.sType								= VK_STRUCTURE_TYPE_GEOMETRY_NV;
	geometry.geometryType						= VK_GEOMETRY_TYPE_TRIANGLES_NV;
	geometry.geometry.triangles.sType			= VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
	geometry.geometry.triangles.vertexData		= vertexBuffer.buffer;
	geometry.geometry.triangles.vertexOffset	= 0;
	geometry.geometry.triangles.vertexCount		= static_cast<uint32_t>(vertices.size());
	geometry.geometry.triangles.vertexStride	= sizeof(Vertex);
	geometry.geometry.triangles.vertexFormat	= VK_FORMAT_R32G32B32_SFLOAT;
	geometry.geometry.triangles.indexData		= indexBuffer.buffer;
	geometry.geometry.triangles.indexOffset		= 0;
	geometry.geometry.triangles.indexCount		= 0;
	geometry.geometry.triangles.indexType		= VK_INDEX_TYPE_UINT32;
	geometry.geometry.triangles.transformData	= VK_NULL_HANDLE;
	geometry.geometry.triangles.transformOffset = 0;
	geometry.geometry.aabbs						= {};
	geometry.geometry.aabbs.sType				= { VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV };
	geometry.flags								= VK_GEOMETRY_OPAQUE_BIT_NV;

	createBottomLevelAccelerationStructure(&geometry);
}

void AccelerationStructure::createBottomLevelAccelerationStructure(const VkGeometryNV* geometries)
{
	VkAccelerationStructureInfoNV accelerationStructureInfo{};
	accelerationStructureInfo.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
	accelerationStructureInfo.type			= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
	accelerationStructureInfo.instanceCount = 0;
	accelerationStructureInfo.geometryCount = 1;
	accelerationStructureInfo.pGeometries	= geometries;

	VkAccelerationStructureCreateInfoNV accelerationStructureCI{};
	accelerationStructureCI.sType	= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
	accelerationStructureCI.info	= accelerationStructureInfo;

	if (vkCreateAccelerationStructureNV(m_Device->getDevice(), &accelerationStructureCI, nullptr, &m_bAccelerationStructure))
		throw std::runtime_error("Error: Failed to create BLAS!");

	VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};
	memoryRequirementsInfo.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
	memoryRequirementsInfo.type						= VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
	memoryRequirementsInfo.accelerationStructure	= m_bAccelerationStructure;

	VkMemoryRequirements2 memoryRequirements2{};
	vkGetAccelerationStructureMemoryRequirementsNV(m_Device->getDevice(), &memoryRequirementsInfo, &memoryRequirements2);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize	= memoryRequirements2.memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex	= findMemoryType(memoryRequirements2.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(m_Device->getDevice(), &memoryAllocateInfo, nullptr, &m_bMemory))
		throw std::runtime_error("Error: Failed to allocate BLAS memory!");

	VkBindAccelerationStructureMemoryInfoNV accelerationStructureMemoryInfo{};
	accelerationStructureMemoryInfo.sType					= VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
	accelerationStructureMemoryInfo.accelerationStructure	= m_bAccelerationStructure;
	accelerationStructureMemoryInfo.memory					= m_bMemory;

	if (vkBindAccelerationStructureMemoryNV(m_Device->getDevice(), 1, &accelerationStructureMemoryInfo))
		throw std::runtime_error("Error: Failed to bind BLAS memory!");

	if (vkGetAccelerationStructureHandleNV(m_Device->getDevice(), m_bAccelerationStructure, sizeof(uint64_t), &m_bHandle))
		throw std::runtime_error("Error: Failed to get BLAS handle!");
}

void AccelerationStructure::createTopGeometry()
{
	VkBuffer instanceBuffer;

	glm::mat3x4 transform = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
	};

	m_Transform = transform;
	m_InstanceId = 0;
	m_Mask = 0xff;
	m_InstanceOffset = 0;
	m_Flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV;
	m_AccelerationStructureHandle = m_bHandle;


}

void AccelerationStructure::createTopLevelAccelerationStructure()
{
	VkAccelerationStructureInfoNV accelerationStructureInfo{};
	accelerationStructureInfo.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
	accelerationStructureInfo.type			= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
	accelerationStructureInfo.instanceCount = 1;
	accelerationStructureInfo.geometryCount = 0;

	VkAccelerationStructureCreateInfoNV accelerationStructureCI{};
	accelerationStructureCI.sType	= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
	accelerationStructureCI.info	= accelerationStructureInfo;

	if (vkCreateAccelerationStructureNV(m_Device->getDevice(), &accelerationStructureCI, nullptr, &m_tAccelerationStructure))
		throw std::runtime_error("Error: Failed to create TLAS!");

	VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};
	memoryRequirementsInfo.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
	memoryRequirementsInfo.type						= VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
	memoryRequirementsInfo.accelerationStructure	= m_tAccelerationStructure;

	VkMemoryRequirements2 memoryRequirements2{};
	vkGetAccelerationStructureMemoryRequirementsNV(m_Device->getDevice(), &memoryRequirementsInfo, &memoryRequirements2);

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize	= memoryRequirements2.memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex	= findMemoryType(memoryRequirements2.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(m_Device->getDevice(), &memoryAllocateInfo, nullptr, &m_tMemory))
		throw std::runtime_error("Error: Failed to allocate TLAS memory!");

	VkBindAccelerationStructureMemoryInfoNV accelerationStructureMemoryInfo{};
	accelerationStructureMemoryInfo.sType					= VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
	accelerationStructureMemoryInfo.accelerationStructure	= m_tAccelerationStructure;
	accelerationStructureMemoryInfo.memory					= m_tMemory;

	if (vkBindAccelerationStructureMemoryNV(m_Device->getDevice(), 1, &accelerationStructureMemoryInfo))
		throw std::runtime_error("Error: Failed to bind TLAS memory!");

	if (vkGetAccelerationStructureHandleNV(m_Device->getDevice(), m_tAccelerationStructure, sizeof(uint64_t), &m_tHandle))
		throw std::runtime_error("Error: Failed to get TLAS handle!");
}

VkDeviceMemory AccelerationStructure::getBottomMemory() const
{
	return m_bMemory;
}

VkDeviceMemory AccelerationStructure::getTopMemory() const
{
	return m_tMemory;
}

VkAccelerationStructureNV AccelerationStructure::getBLAS() const
{
	return m_bAccelerationStructure;
}

VkAccelerationStructureNV AccelerationStructure::getTLAS() const
{
	return m_tAccelerationStructure;
}

uint64_t AccelerationStructure::getBottomHandle() const
{
	return m_bHandle;
}

uint64_t AccelerationStructure::getTopHandle() const
{
	return m_tHandle;
}

uint32_t AccelerationStructure::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_Device->getPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Error: Failed to find suitable memory type!");
}
