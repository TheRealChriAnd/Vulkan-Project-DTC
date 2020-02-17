#pragma once
#include "Defines.h"
#include "glm/glm.hpp"

class DeviceVK;

class AccelerationStructure
{
public:
	AccelerationStructure(DeviceVK* device);
	~AccelerationStructure();
	void createBottomLevelAccelerationStructure(const VkGeometryNV* geometries);
	void createBottomGeometry(uint32_t indexCount, VkBuffer vertexBuffer, VkBuffer index);
	void createTopLevelAccelerationStructure();
	void createTopGeometry();

	VkDeviceMemory getBottomMemory() const;
	VkDeviceMemory getTopMemory() const ;
	VkAccelerationStructureNV getBLAS() const;
	VkAccelerationStructureNV getTLAS() const;
	uint64_t getBottomHandle() const;
	uint64_t getTopHandle() const;

private:
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:
	DeviceVK* m_Device;
	// Ray tracing acceleration structure
	VkDeviceMemory m_bMemory;
	VkDeviceMemory m_tMemory;
	VkAccelerationStructureNV m_bAccelerationStructure;
	VkAccelerationStructureNV m_tAccelerationStructure;
	uint64_t m_bHandle;
	uint64_t m_tHandle;

	struct AccelerationStructure {
		VkDeviceMemory memory;
		VkAccelerationStructureNV accelerationStructure;
		uint64_t handle;
	};

	// Ray tracing geometry instance
	glm::mat3x4 m_Transform;
	uint32_t m_InstanceId : 24;
	uint32_t m_Mask : 8;
	uint32_t m_InstanceOffset : 24;
	uint32_t m_Flags : 8;
	uint64_t m_AccelerationStructureHandle;
	struct GeometryInstance {
		glm::mat3x4 transform;
		uint32_t instanceId : 24;
		uint32_t mask : 8;
		uint32_t instanceOffset : 24;
		uint32_t flags : 8;
		uint64_t accelerationStructureHandle;
	};
};
